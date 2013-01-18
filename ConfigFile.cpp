
/**
ConfigFile handler

Original code by Thibaut CHARLES (crom29 @ hotmail.fr)


This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.





Configuration file example:
________________________________
#Custom comment

#the syntax is :
#	[var name][space][value(s)];
#
#[var name] : Name of the var, without any whitespaces into
#[space] : must be a ' ' or a '\t'
#[value(s)] : the value(s) field
#The ';' is very important !


#Examples:
MyVar 10;
MySentense Hello world !;
MyArray 10 56 38 52 46;

#A value can contain any character excepting ';'
# It can even contain '\n' !

MyMultilineValue Dear sir,
This a message for you

Yours faithfully,
Thibaut CHARLES;

________________________________

**/




#include "ConfigFile.hpp"

#include <fstream>

using namespace std;


/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
std::string ConfigFile::GetErrorString(enum Error e)
{
	if(e==no_error)return "Success";
	else if(e==file_not_found)return "File not found";
	else if(e==file_not_opened)return "The file needs to be opened";
	return "Unknown error :(";
}

/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
ConfigFile::ConfigFile()
{

}

/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
enum ConfigFile::Error ConfigFile::Load(const char* cPath, bool bCreateIfNotExists)
{
	//Open the file
    ifstream File(cPath);

	if(!File)
	{
		if(bCreateIfNotExists)
		{
			//Create the file
			ofstream ofFile(cPath);
			ofFile.close();
		}
		else
		{
			return file_not_found;
		}
	}

    char c;
    do
    {
    	//Go to first interesting char
    	do
    	{
    		File.get(c);
    	}while((c=='\n' || c=='\t' || c==' ') && !File.eof());

		if(!File.eof())
		{
			//It is a comment
			if(c=='#')
			{
				do
				{
					File.get(c);
				}while(c!='\n' && !File.eof());
			}
			//It is a var
			else
			{
				struct ConfigLine CurrConfigLine;

				//Parsing var name
				while(c!=' ' && c!='\t' && !File.eof())
				{
					CurrConfigLine.name+=c;
					File.get(c);
				}

				//Parsing var value
				File.get(c);
				while(c!=';' && !File.eof())
				{
					CurrConfigLine.value+=c;
					File.get(c);
				}

				m_Config.push_back(CurrConfigLine);

			}

		}
    }while(!File.eof());

    m_sPath=cPath;
	return no_error;
}




/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
string ConfigFile::GetStringValue(const char* cName)const
{
    for(unsigned int i=0 ; i<m_Config.size() ; i++)
    {
        if(m_Config[i].name == cName)
            return m_Config[i].value;
    }
    return "";
}

/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
void ConfigFile::SetStringValue(const char* cName, const char* cValue)
{
    //Find occurences in the config
    unsigned int n;
    for(n=0 ; n<m_Config.size() ; n++)
    {
        if(m_Config[n].name == cName)
        {
        	//Set value
        	m_Config[n].value = cValue;
            return;
        }
    }

	//Add new value
	struct ConfigLine ConfigLine;
	ConfigLine.name = cName;
	ConfigLine.value = cValue;

	m_Config.push_back(ConfigLine);
}

/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
enum ConfigFile::Error ConfigFile::WriteToFile(const char* cPath)
{
	ifstream File(m_sPath.c_str());

    if(!File)
    	return file_not_found;


    string sNewFile("");

    bool* bWritten;
    bWritten = new bool[m_Config.size()];
    for(unsigned int i=0 ; i<m_Config.size() ; i++)
		bWritten[i]=false;

    char c;
    do
    {
    	//Go to first interesting char
    	do
    	{
    		File.get(c);
    		if(c=='\n' || c=='\t' || c==' ')
    			sNewFile+=c;
    	}while((c=='\n' || c=='\t' || c==' ') && !File.eof());

		if(!File.eof())
		{
			//It is a comment
			if(c=='#')
			{
				sNewFile+=c;
				do
				{
					File.get(c);
    				sNewFile+=c;
				}while(c!='\n' && !File.eof());
			}
			//It is a var
			else
			{
				string sVarName("");

				//get & rewrite var name
				while(c!=' ' && c!='\t')
				{
					sVarName+=c;
					File.get(c);
				}

				sNewFile+=sVarName;

				//rewrite whitespace
				sNewFile+=c;

				//Ignore var value
				File.get(c);
				while(c!=';')
				{
					File.get(c);
				}
				//Set var value
				for(unsigned int i=0 ; i<m_Config.size() ; i++)
				{
					if(m_Config[i].name == sVarName)
					{
						sNewFile+=m_Config[i].value+c;
						bWritten[i]=true;
					}
				}

			}

		}
    }while(!File.eof());
    File.close();

    //Write unwritten variables
	for(unsigned int i=0 ; i<m_Config.size() ; i++)
	{
		if(bWritten[i]==false)
		{
			sNewFile+=m_Config[i].name+" "+m_Config[i].value+";\n";
		}
	}

	//Writing the new file
	ofstream FileW(cPath);
	FileW<<sNewFile;
	FileW.close();

	return no_error;
}

/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
void ConfigFile::Print()const
{
	for(unsigned int i=0 ; i<m_Config.size() ; i++)
	{
		std::cout<<m_Config[i].name<<"\t"<<m_Config[i].value<<";"<<std::endl;
	}
}


