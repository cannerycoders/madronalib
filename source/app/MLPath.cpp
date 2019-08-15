
// MadronaLib: a C++ framework for DSP applications.
// Copyright (c) 2013 Madrona Labs LLC. http://www.madronalabs.com
// Distributed under the MIT license: http://madrona-labs.mit-license.org/


#include "MLPath.h"
#include "utf.hpp"

namespace ml {

	// parse an input string into our representation: an array of ml::Symbols.
	Path::Path(const char * str)
	{
		parsePathString(str);
	}

	// allocate a path with one symbol.
	Path::Path(const ml::Symbol sym)
	{
		addSymbol(sym);
	}

  Path::Path(const ml::TextFragment frag)
  {
    parsePathString(frag.getText());
  }

  Path::Path(const ml::TextFragment frag, const char separator)
  {
    parsePathString(frag.getText(), separator);
  }

  Path::Path(const Path& a, const Path& b)
  {
    for(Symbol s : a)
    {
      addSymbol(s);
    }
    for(Symbol s : b)
    {
      addSymbol(s);
    }
  }

	void Path::parsePathString(const char* pathStr, const char separator)
	{
		auto it = TextFragment::Iterator(pathStr);		

		size_t symbolSizeInBytes;		
		const char* symbolStartPtr = pathStr;
		
		bool finishedString, charIsSeparator, finishedSymbol;
		do
		{
			symbolSizeInBytes = 0;
			do
			{
				CodePoint cp = *it;
				size_t codePointSize = utf::internal::utf_traits<utf::utf8>::write_length(cp);				
				charIsSeparator = (codePointSize == 1) && (cp == separator);
				finishedString = (cp == '\0');
				finishedSymbol = charIsSeparator || finishedString;
				++it;				
				if(!finishedSymbol)
					symbolSizeInBytes += codePointSize;
			}	
			while(!finishedSymbol);

			addSymbol(ml::Symbol(symbolStartPtr, symbolSizeInBytes));
			symbolStartPtr += symbolSizeInBytes + 1;
		}
		while(!finishedString);
	}

	void Path::addSymbol(ml::Symbol sym)
	{
		if (mSize < kPathMaxSymbols)
		{
			mpData[mSize++] = sym;
		}
		else 
		{
			// TODO something!
			// //debug() << "Path::addSymbol: max path length exceeded!\n";
		}
	}

	ml::Symbol Path::head() const
	{
		return mpData[0];
	}

	Path Path::tail() const
	{
		Path r;
		r.setCopy(getCopy());
		for(int n=1; n<mSize; ++n)
		{
			r.addSymbol(mpData[n]);
		}
		return r;
	}
		
	std::ostream& operator<< (std::ostream& out, const ml::Path & r)
	{
		for(auto sym : r)
		{
			out << "/";
			out << sym;
		}
		unsigned copy = r.getCopy();
		if (copy)
		{
			out << "(#" << copy << ")";
		}
		return out;
	}
} // namespace ml


