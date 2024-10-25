#include <cstdint>
#include <string>
#include <vector>
#include "sqlite3.h"
#include "demoCut.h"



//#define SQLBIND(statement,type,name,value) sqlite3_bind_##type##(statement,sqlite3_bind_parameter_index(statement,name),value)
//#define SQLBIND_NULL(statement,name) sqlite3_bind_null(statement,sqlite3_bind_parameter_index(statement,name))
//#define SQLBIND_TEXT(statement,name,value) sqlite3_bind_text(statement,sqlite3_bind_parameter_index(statement,name),value,-1,NULL)
#define SQLBIND_INDEX(statement,type,index,value) sqlite3_bind_##type##(statement,index,value)
#define SQLBIND_INDEX_NULL(statement,index) sqlite3_bind_null(statement,index)
#define SQLBIND_INDEX_TEXT(statement,index,value) sqlite3_bind_text(statement,index,value,-1,NULL)

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
	union {
		std::string* stringValue = NULL;
		int64_t intValue;
		double doubleValue;
	};
	std::string* columnNameValue = NULL;
public:

	std::string* getColumnName() {
		return columnNameValue;
	}
	
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
		int index = sqlite3_bind_parameter_index(statement, columnNameValue->c_str());
		if (index == 0) {
			return 0; // Sometimes we have same set of binds for multiple statements but with different values being used or not being used. So if index not found, just discard. It doesn't seem like sqlite throws an error even if we use 0 as an index but whatever, safe is safe.
		}
		switch (type) {
		case SQLVALUE_TYPE_NULL:
			return SQLBIND_INDEX_NULL(statement, index);
			break;
		case SQLVALUE_TYPE_INTEGER:
			return SQLBIND_INDEX(statement, int64, index, intValue);
			break;
		case SQLVALUE_TYPE_REAL:
			return SQLBIND_INDEX(statement, double, index, doubleValue);
			break;
		case SQLVALUE_TYPE_TEXT:
			return SQLBIND_INDEX_TEXT(statement, index, stringValue ? stringValue->c_str() : NULL);
			break;
		default:
			throw std::invalid_argument("tried to bind SQLDelayedValue with invalid type");
			break;
		}
	}

	~SQLDelayedValue() {
		if (type == SQLVALUE_TYPE_TEXT && stringValue) {
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
	void inline remove(char* name) {
		size_t valuesCount = values.size();
		for (size_t i = valuesCount-1; i >= 0; i--) {
			if (!_stricmp(values[i]->getColumnName()->c_str(), name)) {
				values.erase(values.begin()+ i);
			}
		}
	}
public:
	template<class T>
	void inline add(char* name, T value) {
		values.push_back(new SQLDelayedValue(name,value));
	}
	template<class T>
	void inline replace(char* name, T value) {
		remove(name);
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