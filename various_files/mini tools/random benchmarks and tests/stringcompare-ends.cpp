/******************************************************************************

Welcome to GDB Online.
  GDB online is an online compiler and debugger tool for C, C++, Python, PHP, Ruby, 
  C#, OCaml, VB, Perl, Swift, Prolog, Javascript, Pascal, COBOL, HTML, CSS, JS
  Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <chrono>
#include <string.h>

// string that can be a constexpr literal and already knows its size and can do epic fast unsafe comparisons
struct CoolString {
	constexpr static size_t maxLen = 100;
	char data[maxLen+1];
	size_t len;
	void printme() {
		std::cout << data << len << "\n";
	}
	// make sure the string ur comparing against is stored in a buffer that is legal to read up to the len of this string+1
	// careful: returns true if match.
	bool unsafematch(const char* cmpstr, int cmpstrlen)const {
		return  !memcmp(data,cmpstr,len+1);  // adding cmpstrlen == len &&  actually makes this 10 times slower!!
	}
	bool isending(const char* cmpstr, int cmpstrlen)const {
		return  !memcmp(data,cmpstr+cmpstrlen-len,len+1);  // adding cmpstrlen == len &&  actually makes this 10 times slower!!
	}
};


constexpr CoolString operator ""_cs(const char* blah,size_t len) {
	CoolString retVal{};
	if (len > CoolString::maxLen) {
		throw std::invalid_argument("_cs string literal cant be longer than 20 chars");
	}
	//constexpr len = strlenConstExpr(blah);
	for (size_t i = 0; i < len; i++) {
		retVal.data[i] = blah[i];
	}
	retVal.data[len] = 0;
	retVal.len = len;
	return retVal;
}

const char strings[][100]={
    "test",
    "test2",
    "@@@TIMELIMIT_HIT. ^4Blue^7 won the match.",
    "Blue @@@HIT_THE_KILL_LIMIT",
    "^4b^3abc^3_^7 @@@HIT_THE_KILL_LIMIT.",
    "Padawinner^7 hit the win limit.",
    "^Join.us.now^7 hit the win limit.",
    "Blue hit the capturelimit.",
    "Red hit the capturelimit.",
    "^4BLUE ^7hit the capturelimit.",
    "^1RED ^7hit the capturelimit.",
    "@@@PRINTREDTEAM   ",
    "@@@HIT_CAPTURE_LIMIT.",
    "@@@PRINTBLUETEAM  ",
    "@@@HIT_CAPTURE_LIMIT ^7- Team ^4BLUE ^7leads by ^27^7.",
	"Can't follow a spectator.",
	"<no players to list>",
	"Unknown command ~ammodinfo~",
	"Unknown command ~ammodinfo_twitch~",
	"Unknown command ~serverstatus~",
	"Unknown command ~clientinfo~",
	"Unknown command ~clientlist~",
	"Game was paused.",
	"Pause ended after 2m28s.",
	"Pause ended after 1m38s.",
	"Pause ended after 1m13s.",
	"Blue hit the capturelimit.",
	"You cannot perform this task during the intermission.",
	"Pause ended after 1m32s.",
	"Red hit the capturelimit.",
	"Server: fraglimit changed to 100",
	"Red @@@HIT_THE_KILL_LIMIT",
	"Blue @@@HIT_THE_KILL_LIMIT",
	"ADMIN ABUSERS                     ",
	"All were muted.",
	"All were unmuted.",
	"BUILD DATE: Apr 21 2024 | 12:11:48",
	"Blue @@@HIT_THE_KILL_LIMIT",
	"Can't follow a spectator.",
	"Can't follow yourself.",
	"Could not find client '11'",
	"Could not find client '12'",
	"Could not find client '13'",
	"Could not find client '15'",
	"Could not find client '16'",
	"Could not find client '18'",
	"Pause ended after 1m23s.",
	"Pause ended after 1m24s.",
	"Pause ended after 1m28s.",
	"You cannot perform this task during the intermission.",
};
const size_t stringcount = sizeof(strings)/sizeof(strings[0]);
int stringLengths[stringcount];

void compareStrcmp(){
    int matches = 0;
    for(int r=0;r<100000;r++){
        for(int i=0;i<stringcount;i++){
            matches += !strcmp("mit.",strings[i]+stringLengths[i]-4);
            //matches += !memcmp("Red hit the capturelimit.",strings[i],26);
        }
    }
    std::cout<<matches<<" matches\n";
}
void compareStrcasecmp(){
    int matches = 0;
    for(int r=0;r<100000;r++){
        for(int i=0;i<stringcount;i++){
            matches += !strcasecmp("mit.",strings[i]+stringLengths[i]-4);
            //matches += !memcmp("Red hit the capturelimit.",strings[i],26);
        }
    }
    std::cout<<matches<<" matches\n";
}
void compareMine(){
    int matches = 0;
    for(int r=0;r<100000;r++){
        for(int i=0;i<stringcount;i++){
            
			static constexpr auto theStrng = "mit."_cs;
            matches += theStrng.isending(strings[i],stringLengths[i]);
        }
    }
    std::cout<<matches<<" matches\n";
}

void measureStrcasecmp(){
    
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    auto t1 = high_resolution_clock::now();
    compareStrcasecmp();
    auto t2 = high_resolution_clock::now();

    /* Getting number of milliseconds as an integer. */
    auto ms_int = duration_cast<milliseconds>(t2 - t1);

    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;

    //std::cout << ms_int.count() << "ms\n";
    std::cout << ms_double.count() << "ms\n";

}
void measureStrcmp(){
    
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    auto t1 = high_resolution_clock::now();
    compareStrcmp();
    auto t2 = high_resolution_clock::now();

    /* Getting number of milliseconds as an integer. */
    auto ms_int = duration_cast<milliseconds>(t2 - t1);

    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;

    //std::cout << ms_int.count() << "ms\n";
    std::cout << ms_double.count() << "ms\n";

}
void measuremine(){

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    auto t1 = high_resolution_clock::now();
    compareMine();
    auto t2 = high_resolution_clock::now();

    /* Getting number of milliseconds as an integer. */
    auto ms_int = duration_cast<milliseconds>(t2 - t1);

    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;

    //std::cout << ms_int.count() << "ms\n";
    std::cout << ms_double.count() << "ms\n";

}

int main()
{	
	"Red hit the capturelimit."_cs.printme();
    for(int i=0;i<stringcount;i++){
        stringLengths[i] = strlen(strings[i]);
    }
    std::cout << "\nstrcasecmp: \n";
    measureStrcasecmp();
    std::cout << "\nstrcmp: \n";
    measureStrcmp();
    std::cout << "\nmine: \n";
    measuremine();
    return 0;
}