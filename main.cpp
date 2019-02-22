#include <iostream>

#include "code_feed.hpp"
#include "frame.hpp"

/*
 * I'm rewriting my old scripting language (YodaScript). Looking back because i made it without a plan,
 * i had to add features in weird ways to make them compatible with the interpreter (ie - being forced to
 * make random intermediate datatypes which had to be lazy evaluated because references were only based on
 * variable names). Due to how complex and interconnected different parts of the interpreter were I was
 * pretty much forced into a complete rewrite. Either way, should be fun. :)
 *
 *

Important internal implementation changes:
- no more 2.5k line if statement :)
- no more linked lists
- everything associated with a given scope will be contained in and handled by a Frame object
- no more running out of tempfiles
- removal of 2-3k lines of code for lazy evaluation

Important langauge changes:
- references are now technically pointers
- all variables are references to data (remember `~` == valueof)
- objects will prolly get syntax changes and improvements
  + also proper sense of self
- operator overloading (based on python)
- lambda argument handling will get changed
  + missing args handler and va_args were cool, but slow, so

 Objects likely will be seeing syntax changes, also gaining *real* sense of self
- plans for things like operator overloading (probably similar to python)
- special lambda argument handlers will probably be removed in order to improve performance
    + likely be replaced by an args value containing everything passed to the lambda (ty js)
    + lambdas won't throw when args length isn't correct


Ideas I'd like to explore but am currently unsure about:
- Multi-threading: I designed the new interpreter to where this would be an option. However I don't have much experience with this and I know that adding mutex's and stuff to make the interpreter threadsafe would reduce performance slightly
- namespaces
- functions associated with native/primitive datatypes
     + right now it doesnt make sense for primitive's to have member functions because they arent objects
     + but should it?
     + currently thinking about something like "hello" str:len => 5, but might do something else
- goto's: with new interpreter design these aren't impossible anymore. (still kinda silly tho)
 *
 *
 *
 */
int main(int argc, char** argv) {


	if (argc == 1) {

		Frame frame;
		frame.feed.isStdin = true;
		frame.feed.getLine();

		while (true) {
			Frame::Exit e = frame.run();
			if (e.reason == Frame::Exit::ERROR) {
				std::cout <<"Error caught: ("<<e.c_num <<") "<< e.title <<": " <<e.desc <<std::endl;
			}
			frame.feed.getLine();
		}
	} else if (argc == 2) {
		if (**(argv+1) != '-') {
			std::cout <<"Running file...\n";
			Frame main;
			main.feed.loadFile(argv[1]);
			Frame::Exit e = main.run();

		}

	}
}