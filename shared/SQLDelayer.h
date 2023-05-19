#include <cstdint>
#include <string>
#include <vector>
#include "sqlite3.h"
#include "demoCut.h"



#define SQLBIND(statement,type,name,value) sqlite3_bind_##type##(statement,sqlite3_bind_parameter_index(statement,name),value)
#define SQLBIND_NULL(statement,name) sqlite3_bind_null(statement,sqlite3_bind_parameter_index(statement,name))
#define SQLBIND_TEXT(statement,name,value) sqlite3_bind_text(statement,sqlite3_bind_parameter_index(statement,name),value,-1,NULL)

enum SQLDelayedValueType {
	SQLVALUE_TYPE_NULL,
	SQLVALUE_TYPE_INTEGER,
	SQLVALUE_TYPE_REAL,
	SQLVALUE_TYPE_TEXT,
};

typedef enum SQLDelayedValue_NULL_t {
	SQLDelayedValue_NULL
};

// One could argue this is a little problematic because sometimes we may provide an integer to a double field and here 
// that wont result in a C++ side implicit conversion but SQLite will automatically do such conversions anyway so I guess who cares
// Does this even make sense? Would C++ even allow this? I'm not sure, I'm tired.
class SQLDelayedValue { 
	SQLDelayedValueType type = SQLVALUE_TYPE_NULL;
	int64_t intValue = 0;
	double doubleValue = 0;
	std::string* columnNameValue = NULL;
	std::string* stringValue = NULL;
public:

	
	template<class T>
	SQLDelayedValue(char* columnName, T valueA) {
		if constexpr (std::is_floating_point<T>()) {
			doubleValue = valueA;
			type = SQLVALUE_TYPE_REAL;
		}
		else if constexpr (std::is_same<T, SQLDelayedValue_NULL_t>()) {
			if (valueA) {
				throw std::invalid_argument("null constructor must be called with value SQLDelayedValue_NULL (0)");
			}
			type = SQLVALUE_TYPE_NULL;
		}
		else if constexpr (std::is_integral<T>() || std::is_enum<T>() || std::is_same<T, bool>()) {
			intValue = valueA;
			type = SQLVALUE_TYPE_INTEGER;
		} else if constexpr (std::is_same<T,char*>() || std::is_same<T, const char*>()) {
			stringValue = valueA ? new std::string(valueA) : NULL;
			type = SQLVALUE_TYPE_TEXT;
		} else if constexpr (std::is_same<T, std::string>()) {
			stringValue = new std::string(valueA);
			type = SQLVALUE_TYPE_TEXT;
		} else if constexpr (std::is_same<T, std::string*>()) {
			stringValue = new std::string(*valueA);
			type = SQLVALUE_TYPE_TEXT;
		}
		else {
			throw std::invalid_argument("Invalid SQLDelayedValue constructor type");
		}
		columnNameValue = new std::string(columnName);
	}


	inline int bind(sqlite3_stmt* statement) {
		switch (type) {
		case SQLVALUE_TYPE_NULL:
			return SQLBIND_NULL(statement, columnNameValue->c_str());
			break;
		case SQLVALUE_TYPE_INTEGER:
			return SQLBIND(statement, int, columnNameValue->c_str(), intValue);
			break;
		case SQLVALUE_TYPE_REAL:
			return SQLBIND(statement, double, columnNameValue->c_str(), doubleValue);
			break;
		case SQLVALUE_TYPE_TEXT:
			return SQLBIND_TEXT(statement, columnNameValue->c_str(), stringValue ? stringValue->c_str() : NULL);
			break;
		default:
			throw std::invalid_argument("tried to bind SQLDelayedValue with invalid type");
			break;
		}
	}

	~SQLDelayedValue() {
		if (stringValue) {
			delete stringValue;
		}
		if (columnNameValue) {
			delete columnNameValue;
		}
	}
};

class SQLDelayedQuery {
	std::vector<SQLDelayedValue*> values;
	// The added SQLDelayedValue object will be automatically deleted when this object is destroyed.
	//void add(SQLDelayedValue* value) {
	//	values.push_back(value);
	//}
public:
	template<class T>
	void inline add(char* name, T value) {
		values.push_back(new SQLDelayedValue(name,value));
	}
	inline void bind(sqlite3_stmt* statement) {
		int count = values.size();
		for (int i = 0; i < count; i++) {
			values[i]->bind(statement);
		}
	}
	~SQLDelayedQuery() {
		int count = values.size();
		for (int i = 0; i < count; i++) {
			delete values[i];
		}
	}
};

class SQLDelayer {
	std::vector<SQLDelayedQuery*> queries;
	
public:
	// No need to delete this yourself afterwards.
	// SQLDelayer will delete all these objects on its own when destroyed.
	inline void add(SQLDelayedQuery* query) {
		queries.push_back(query);
	}
	inline void execute(sqlite3_stmt* statement) {
		int count = queries.size();
		for (int i = 0; i < count; i++) {
			queries[i]->bind(statement);
		}
	}
	~SQLDelayer() {
		int count = queries.size();
		for (int i = 0; i < count; i++) {
			delete queries[i];
		}
	}

};