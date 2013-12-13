// local:
#include "utils.h"

// STL:
using namespace std;

float randomFloat( float low, float high )
{
	return low + ( (high-low)*rand() ) / RAND_MAX;
}

int randomInt( int low, int high )
{
	return low + rand()%(high-low+1);
}

vector< string > tokenizeString( const string& str, const string& delimiters )
{  
   vector<string> tokens;
   // Skip delimiters at beginning.
   string::size_type lastPos = str.find_first_not_of(delimiters, 0);
   // Find first "non-delimiter".
   string::size_type pos = str.find_first_of(delimiters, lastPos);

   while (string::npos != pos || string::npos != lastPos)
    {  // Found a token, add it to the vector.
      tokens.push_back(str.substr(lastPos, pos - lastPos));
      // Skip delimiters.  Note the "not_of"
      lastPos = str.find_first_not_of(delimiters, pos);
      // Find next "non-delimiter"
      pos = str.find_first_of(delimiters, lastPos);
   }
    return tokens;
}
