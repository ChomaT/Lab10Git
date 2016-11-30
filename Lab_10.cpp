
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

/*
 * File:   Lab_10.cpp
 * Author: theoc_000
 *
 *
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>



using namespace std;

class message{

    protected:
        string msg;
    public:
        message();
        message(string);
        ~message();
        void print();

};


message::message(){
    cout<<"Please input a message to translate: "<<endl;
    cin>>msg;
}

message::message(string input){
    msg = input;
}

message::~message(){
	cout << "\nDestructorMSG" << endl;
}

void message::print(){
    cout << msg <<endl;
}


class morseCodeMessage:public message{
    public:
        string tran_msg;
        int index;
        morseCodeMessage();
        morseCodeMessage(string);
        ~morseCodeMessage();
        void translate(string message1);
        void print();
        void morseToLED();
};

morseCodeMessage::morseCodeMessage() : message(){
    translate(msg);
}

morseCodeMessage::morseCodeMessage(string input) : message(input){
    msg = input;
    translate(msg);
}

morseCodeMessage::~morseCodeMessage(){
	cout << "\nDestructorMorse" << endl;
}

void morseCodeMessage::print(){
    cout << msg << endl;
    cout << tran_msg << endl;
    //print morse code part
}


void morseCodeMessage::translate(string message1) {
    //translate english to morse
	string morseCode[] = {".-", "-...", "-.-.", "-..", ".", "..-.",
		"--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---",
		".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-",
		"-.--", "--.."};
		char ch;
		string morseWord = "";

		for(unsigned int i=0; i < message1.length(); i++)
		{
			ch = message1[i];
			ch = toupper(ch);
			morseWord += morseCode[ch - 'A'];
			morseWord += " ";
		}
		tran_msg = morseWord;
}

void morseCodeMessage::morseToLED(){
	int fd;		// for the file descriptor of the special file we need to open.
			unsigned long *BasePtr;		// base pointer, for the beginning of the memory page (mmap)
			unsigned long *PBDR, *PBDDR;	// pointers for port B DR/DDR
		fd = open("/dev/mem", O_RDWR|O_SYNC);	// open the special file /dem/mem
			if(fd == -1){
				printf("\n error\n");
			}

			// We need to map Address 0x80840000 (beginning of the page)
			BasePtr = (unsigned long*)mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0x80840000);
			if(BasePtr == MAP_FAILED){
				printf("\n Unable to map memory space \n");
			}  // failed mmap

			// To access other registers in the page, we need to offset the base pointer to reach the
			// corresponding addresses. Those can be found in the board's manual.
			PBDR = BasePtr + 1;		// Address of port B DR is 0x80840004
			PBDDR = BasePtr + 5;	// Address of port B DDR is 0x80840014

			*PBDDR |= 0xE0;			// configures port B5-7 as outputs (Green-Yellow-Red LEDs)
			*PBDDR &= 0xFFFFFFFE;	// configures port B0 as input (first push button). You could use: &= ~0x01
			*PBDR &= ~0x20;	// OFF: write a 0 to port B0. Mask all other bits. (Red)
			*PBDR &= ~0x80;	// OFF: write a 0 to port B0. Mask all other bits. (Green)
			*PBDR &= ~0x40;	// OFF: write a 0 to port B0. Mask all other bits. (Yellow)



	int i = 0, j = 0;
	for(i=0; i < msg.length(); i++)
	{
		while(tran_msg[j] != ' ')
		{
			if(tran_msg[j] == '.')
			{
				*PBDR |= 0x20;	// ON: write a 1 to port B0. Mask all other bits.
					sleep(1);
				*PBDR &= ~0x20;	// OFF: write a 0 to port B0. Mask all other bits.
					sleep(1);
			}
			else if(tran_msg[j] == '-')
			{
				*PBDR |= 0x40;	// ON: write a 1 to port B0. Mask all other bits.
					sleep(1);
				*PBDR &= ~0x40;	// OFF: write a 0 to port B0. Mask all other bits.
					sleep(1);
			}
			j++;
		}
		j++;
		sleep(2);
	}
	*PBDR |= 0x80;	// ON: write a 1 to port B0. Mask all other bits.
				sleep(1);
			*PBDR &= ~0x80;	// OFF: write a 0 to port B0. Mask all other bits.
				sleep(1);
}

class messageStack{
public:
    message *ptrstack[10];
    messageStack(message);
    ~messageStack();
    void push(message current_obj);
    void pop();
    void printStack();
    int stack_top;
    int numobj;
};

messageStack::messageStack(message init){
	cout<<"constructor"<<endl;
	ptrstack[0] = new message(init);

    numobj= 1;
    stack_top = 1;


}
messageStack::~messageStack(){
	for(int i = 0; i < numobj; i++)
	{
		delete ptrstack[i];
	}
	delete[] ptrstack;	// instead of 'free'
	cout << "\nDestructorStack" << endl;
}


void messageStack::push(message current_obj){
    ptrstack[stack_top] = new message(current_obj);
	numobj++;
	stack_top++;
}

void messageStack::pop(){
    delete ptrstack[stack_top--];
	numobj--;
}

void messageStack::printStack(){
    //print all messages from top to bottom of stack
	//ptrstack = stack_top;			//set pointer to top of stack
	cout<<"print stack fn"<< endl;
	for(int i = 0; i < numobj; i++)
	{
		cout<<"Message no. " << i <<endl;
		ptrstack[i]->print();
	}
}

int main(int argc, char** argv) {







	message msg1("test");
    msg1.print();
    morseCodeMessage msg2("tester");
    msg2.print();
    msg2.morseToLED();

   // messageStack stack(msg1);
   // stack.push(msg2);
   // stack.printStack();
   // stack.pop();
   // stack.printStack();


    cout<<"exit"<<endl;
    return 0;
}

