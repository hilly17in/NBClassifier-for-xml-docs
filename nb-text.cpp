// nb-text.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define BUFFER_LENGTH	1024
char* DirPath = NULL;

typedef struct _prob
{
	double c1;	// prob of being in class 1
	double c2;	// prob of being in class 2
}prob;

// global variables..
vector<string> stopwords;
string delim;
vector<map<string,int>> tfmapvec;

// function to match argument
template <typename T1, typename T2>
struct less_second {
    typedef pair<T1, T2> type;
    bool operator ()(type const& arg1, type const& arg2) const {
        return arg1.second < arg2.second;
    }
};

void Tokenize(const string& str,
                      vector<string>& tokens,
                      const string& delimiters = " ");

void getDelim(string &str);
bool checkalpha(char ch);
bool isnumber(string str);
void getStopwords(string filename);
void GetDatasetTokens(string datasetpath, vector<string> &dataset_tokens);
void GetDFMap(vector<string> datasets, map<string,int> &dfmap, unsigned int &maxd);
void GetMapFromTokens(vector<string> &dataset_tokens, map<string,int> &tfmap);

unsigned int CreateDictionary(vector<string> datasets, set<string> &dict)
{
	
	vector<string>::iterator ds_itr; /* to enumerate on vector of paths of datasets */

	vector<string> dataset_tokens;
	map<string,int> tfmap;
	vector<string>::iterator str_itr;

	ds_itr = datasets.begin();

	while(ds_itr != datasets.end())
	{
		dataset_tokens.clear();
		tfmap.clear();
		
		/* tokens from dataset */
		GetDatasetTokens(*ds_itr, dataset_tokens);
		
		/*tf map from these tokens */
		//GetMapFromTokens(dataset_tokens, tfmap);

		/* push the map into vector */
		//tfmapvec.push_back(tfmap);

		str_itr = dataset_tokens.begin();

		/* insert the tokens into single dictionary */
		while(str_itr != dataset_tokens.end())
		{
			// insert into set.. 
			// need not worry for duplicates as set takes care
			dict.insert(*str_itr);
			str_itr++;
		}

		// iterate to next dataset	
		ds_itr++;
	}
	
	cout<<"created dictionary:1"<<endl;
	//getchar();
	
	// removing stopwords from dictionary..
	vector<string>::iterator stopitr = stopwords.begin();
	set<string>::iterator tm;
	while(stopitr != stopwords.end())
	{
		tm = dict.find(*stopitr);

		if(tm != dict.end())
			dict.erase(tm);

		stopitr++;
	}
	cout<<"removed stopwords"<<endl;
	//getchar();


	/* 
		find document frequency of each term...	
		so that >50% can be removed..
	*/
	
	map<string,int> dfmap;
	set<string>::iterator itrx = dict.begin();
 
	// creating a map from set so that we can calculate df for each term..
	while(itrx != dict.end())
	{
		dfmap.insert(pair<string,int>(*itrx, 0));
		itrx++;
	}

	cout << " created df map" <<endl;

	unsigned int maxd;
	// fill up the dfmap with df for each term.
	// maxd is also received = how many articles in collection.
	GetDFMap(datasets, dfmap, maxd);
	cout << " created df map with freq" <<endl;
	fstream out_df;
	out_df.open("dfmap.txt", ios::out);
	map<string, int>::iterator itrmap = dfmap.begin();

	int half = maxd / 2;

	//getchar();
	while(itrmap != dfmap.end())
	{
		out_df << itrmap->first <<";" << itrmap->second << endl;

		// if term occurs more than 50% of articles in collection
		if(itrmap->second > half)
		{
			//cout << "removing .. " << itrmap->first << endl;
			dict.erase(itrmap->first);
		}
		itrmap++;
	}

	out_df.close();

	//getchar();
	return 0;
}

void ViewDictionary(set<string> &dict)
{
	fstream filedict;
	set<string>::iterator itr;
	
	itr = dict.begin();
	filedict.open("dictionary.txt", ios::out);

	cout<< "the dictionary is as follows:" << endl;
	//getchar();
	while(itr != dict.end())
	{
		filedict << *itr << endl;
		itr++;
	}

	filedict.close();
}

void RemoveIfNotinDictionary(map<string,int> &tfmap, set<string> &dict)
{
	map<string,int>::iterator itr_map;
	map<string,int>::iterator itr_end;
	set<string>::iterator fo;

	itr_map = tfmap.begin();
	itr_end = tfmap.end();

	while(itr_map != itr_end)
	{
		fo = dict.find(itr_map->first);
		if(fo == dict.end()) // it means not found in dictionary, so remove it from map
		{
			itr_map = tfmap.erase(itr_map);
			itr_end = tfmap.end();
		}
		else
		{
			itr_map++;
		}
	} // while
} // function ends


void DumpToFile(string filename, map<string,int> tfmap)
{
	fstream out_tf;
	
	out_tf.open(filename.c_str(), ios_base::out);
		
	map<string,int>::iterator itr_map = tfmap.begin();
	while(itr_map != tfmap.end())
	{
		
		out_tf << itr_map->first << ";" << itr_map->second << endl;

		itr_map++;		
	} // while

	out_tf << "Total elements = " << tfmap.size() << endl;
	out_tf.close();
}

void Train(unsigned int v, set<string> &dict, string dataset1, string dataset2)
{
	/* For Dataset1 */
	vector<string> dataset_tokens1;
	map<string,int> tfmap1;

	GetDatasetTokens(dataset1, dataset_tokens1);
	cout << "ds1- tokenizecomplete" << endl;

	GetMapFromTokens(dataset_tokens1, tfmap1);
	cout << "ds1- created a map" << endl;

	RemoveIfNotinDictionary(tfmap1, dict);
	DumpToFile("tfmap1.txt", tfmap1);

	vector<string> dataset_tokens2;
	map<string,int> tfmap2;
	
	GetDatasetTokens(dataset2, dataset_tokens2);
	cout << "ds2- tokenize complete.." << endl;

	GetMapFromTokens(dataset_tokens2, tfmap2);
	cout << "ds2- created tf map" << endl;

	RemoveIfNotinDictionary(tfmap2, dict);
	DumpToFile("tfmap2.txt", tfmap2);
	cout << "elements in tfmap2: " << tfmap2.size() << endl;

	unsigned int count1=0;
	unsigned int count2=0;
	double p1d=0;
	double p1=0;
	double p2=0;
	double p2d=0;
	unsigned int tf1=0;
	unsigned int tf2=0;
	
	map<string,int>::iterator itrmap;

	itrmap = tfmap1.begin();
	while(itrmap!=tfmap1.end())
	{
		count1 += itrmap->second;
		itrmap++;
	}
	itrmap = tfmap2.begin();
	while(itrmap!=tfmap2.end())
	{
		count2 += itrmap->second;
		itrmap++;
	}

	fstream model;
	model.open("model.txt", ios::out);

	set<string>::iterator itr_dict;
	itr_dict = dict.begin();
	//getchar();

	cout.precision(5);
	while(itr_dict != dict.end())
	{
		itrmap = tfmap1.find(*itr_dict);
		if(itrmap == tfmap1.end())
			tf1 = 0;
		else
			tf1 = itrmap->second;

		p1d = (double)(tf1+1)/(count1+v) ;
		p1 = log10(p1d);

		itrmap = tfmap2.find(*itr_dict);
		if(itrmap == tfmap2.end())
			tf2 = 0;
		else
			tf2 = itrmap->second;

		p2d = (double)(tf2+1)/(count2+v) ;
		p2 = log10(p2d);

		cout << *itr_dict << "," << p1 << "," << p2 << endl;
	
		model << *itr_dict << " " << p1 << " " << p2 << endl;
		itr_dict++;
	}
	
		
	model.close();
}


void GetTokensfromFile(string filename, vector<string> &tokens)
{
	string data;
	vector<string> toks;

	// read into data from <TEXT> of XML FILE
	xmlRead(filename, data);
	toks.clear();

	// now tokenize, delim is global..
	Tokenize(data, toks, delim);

	// convert all tokens to lower case characters
	vector<string>::iterator itr_low = toks.begin();
	while(itr_low != toks.end())
	{
		std::transform( (*itr_low).begin(), (*itr_low).end(), (*itr_low).begin(), ::tolower);
		itr_low++;
	}


	tokens.clear();
	// stemming
	vector<string>::iterator itr_stm = toks.begin();
	while(itr_stm != toks.end())
	{
		std::wstring tempw(itr_stm->begin(), itr_stm->end());

		stemming::english_stem<> StemEnglish;
		StemEnglish(tempw);

		std::string temp(tempw.begin(), tempw.end());
		tokens.push_back(temp);
		itr_stm++;
	}

}

void OpenModelFile(string filename, map<string,prob> &model)
{
	ifstream modelfile;
	modelfile.open(filename.c_str(), ios::in);
	string term;
	double pc1;
	double pc2;

	unsigned int x = 0;

	while(true)
	{
		prob p;
		modelfile >> term >> pc1 >> pc2 ;
		p.c1 = pc1;
		p.c2 = pc2;

		if(x%17 == 0)
			cout << term << ";" << pc1 << ";" << pc2 << endl;

		model.insert(pair<string,prob>(term,p));
		if(modelfile.eof())
			break;

		x++;
	}

	modelfile.close();
	getchar();
}

void Test()
{
	// just convert char* to string..
	string cppDirPath = DirPath;

	unsigned int tp1=0;
	unsigned int tp2=0;
	double acc1, acc2, acc;

	// construct the dataset path (s)
	// ideal way can be to enumerate recursive..
	vector<string> datasets;
	string dataset1;
	string dataset2;

	dataset1 = cppDirPath + "\\" + "Test" + "\\" + "Class1";
	dataset2 = cppDirPath + "\\" + "Test" + "\\" + "Class2";
	cout << "the dataset 1 is " << dataset1 << endl;
	cout << "the dataset 2 is " << dataset2 << endl;

	vector<string> testCollection;
	// testing for class 1..
	
	// for enumeration of all files in the directory..
	CListDir listDataset, listDataset2;
	listDataset.SetDirectoryPath(dataset1);
	listDataset2.SetDirectoryPath(dataset2);
	listDataset.listDir("");
	listDataset2.listDir("");
	
	testCollection.insert(testCollection.end(), listDataset.filenames.begin(), listDataset.filenames.end());
	testCollection.insert(testCollection.end(), listDataset2.filenames.begin(), listDataset2.filenames.end());

	vector<string> filetokens;
	vector<string>::iterator itr_tokens;
	vector<string>::iterator itr_filenames ;
	double score1=0.0;
	double score2=0.0;

	map<string,prob> nbmodel;
	map<string,prob>::iterator itr_model;
	OpenModelFile("model.txt", nbmodel);


	/* result file */
	fstream result;
	result.open("results.txt", ios::out);
	unsigned int numClass1=0;
	unsigned int numClass2=0;
	unsigned int numnoclass=0;


	// test dataset1 - every document is this dataset is actually class1
	// so score1>score2 is good..
	//
	itr_filenames = listDataset.filenames.begin();
	while(itr_filenames != listDataset.filenames.end())
	{
		filetokens.clear();
		score1 = log10(0.5);	// hard coding 
		score2 = log10(0.5);	// hard coding
		GetTokensfromFile(*itr_filenames, filetokens);
		
		cout << "got tokens" << endl;
		itr_tokens = filetokens.begin();
		while(itr_tokens != filetokens.end()) // process each token and update score..
		{
			itr_model = nbmodel.find(*itr_tokens);
			if(itr_model != nbmodel.end())
			{
				score1 = score1 + itr_model->second.c1 ;
				score2 = score2 + itr_model->second.c2 ;
			}
			itr_tokens++;
		}
		if(score1 > score2){
			result << *itr_filenames << "," << score1 <<"," << score2 << "," << "class1" << endl;
			tp1++;
		}
		else if (score2 > score1){
			result << *itr_filenames << "," << score1 <<"," << score2 << "," << "class2" << endl;
			numClass2++;
		}
		else
		{
			numnoclass++;
		}
		itr_filenames++;
	} // while

	// test dataset2 - every document is this dataset is actually class1
	// so score1>score2 is good..
	//
	itr_filenames = listDataset2.filenames.begin();
	while(itr_filenames != listDataset2.filenames.end())
	{
		filetokens.clear();
		score1 = log10(0.5);	// hard coding 
		score2 = log10(0.5);	// hard coding
		GetTokensfromFile(*itr_filenames, filetokens);
		
		cout << "got tokens" << endl;
		itr_tokens = filetokens.begin();
		while(itr_tokens != filetokens.end()) // process each token and update score..
		{

			itr_model = nbmodel.find(*itr_tokens);
			if(itr_model != nbmodel.end())
			{
				score1 = score1 + itr_model->second.c1 ;
				score2 = score2 + itr_model->second.c2 ;
			}

			itr_tokens++;
		}
	
		if(score1 > score2){
			result << *itr_filenames << "," << score1 <<"," << score2 << "," << "class1" << endl;
			numClass1++;
		}
		else if (score2 > score1){
			result << *itr_filenames << "," << score1 <<"," << score2 << "," << "class2" << endl;
			tp2++;
		}
		else
		{
			numnoclass++;
		}

		itr_filenames++;
	}

	acc1 = (double)tp1 / 50;
	acc2 = (double)tp2 / 50;
	acc = (double)(tp1+tp2) / 100;
	cout << "acc1 = " << acc1 << ", acc2 = " << acc2 << ", acc = " << acc << endl;

}

int _tmain(int argc, _TCHAR* argv[])
{
	// Local Variables
	DWORD dwRet = 0;
	DWORD BufLen = BUFFER_LENGTH;
	int flagdirpath = 0;
	int flagtrain = -1; // 0=train and 1=test

	/*base directory*/
	DirPath = (char*)malloc(BUFFER_LENGTH);
	if (DirPath == NULL) {
		cout << "not enough memory to initialize. \n";
		return 0;
	}

	vector<string> vecargs;
	vector<string>::iterator itr;
	for (int i = 0; i < argc; i++)
		vecargs.push_back(argv[i]);

	itr = find(vecargs.begin(), vecargs.end(), "--dirpath");
	if (itr != vecargs.end())
	{
		if (itr + 1 != vecargs.end())
			strcpy_s(DirPath, BUFFER_LENGTH - 1, (*(itr + 1)).c_str());
		else
			flagdirpath = 1;
	}
	else
		flagdirpath = 1;
	
	if (flagdirpath)
	{
		cout << "not enough arguments" << endl;
		return 0;
	}

	itr = find(vecargs.begin(), vecargs.end(), "--train");
	if (itr != vecargs.end())
		flagtrain = 0;
	
	itr = find(vecargs.begin(), vecargs.end(), "--test");
	if (itr != vecargs.end())
		flagtrain = 1;

	if (flagtrain == -1) {
		cout << "Not enough arguments specified !" << endl;
		return 0;
	}

	// just convert char* to string..
	string cppDirPath = DirPath;

	// construct the dataset path (s)
	// ideal way can be to enumerate recursive..
	vector<string> datasets;
	string dataset1;
	string dataset2;

	dataset1 = cppDirPath + "\\" + "Train" + "\\" + "Class1";
	dataset2 = cppDirPath + "\\" + "Train" + "\\" + "Class2";
	cout << "the dataset 1 is " << dataset1 << endl;
	cout << "the dataset 2 is " << dataset2 << endl;
	
	datasets.push_back(dataset1);
	datasets.push_back(dataset2);

	// get the stop words from the file.
	// stopwords is global variable;
	string stopwordpath ;
	stopwordpath = cppDirPath + "\\" + "stop-words-english.txt";
	getStopwords(stopwordpath);
	cout << "got stop words" << endl;
	//getchar();

	//Assume that any non alpha character is a delimiter.. 
	// delim is global variable;
	getDelim(delim);
	cout << "got delim chars" << endl;
		

	if (flagtrain == 0)
	{
		// 1. CreateDictionary
		set<string> dictionary;
		unsigned int dict_size; // |V| 

		CreateDictionary(datasets, dictionary);
		ViewDictionary(dictionary);
		dict_size = dictionary.size();
		cout << "dictionary dumped" << endl;

		// 2. TrainClassifier
		//
		//P(term|class1) = (tf + 1) / (countTerms + |v|)
		//foreach term in dict, find tf.
		//we have term freq for all,
		//remove items..

		//before train, we must make a file/map where we store each term and its freq.
		//its available in tfmapvec
		Train(dict_size, dictionary, dataset1, dataset2);
	}
	
	else if (flagtrain == 1)
	{
		// 3. TestClassifier
		Test();
	}

	return 0;
}


void Tokenize(const string& str,
                      vector<string>& tokens,
                      const string& delimiters)
{
    // Skip delimiters at beginning
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);

    // Find first non-delimiter
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }


	// remove stopwords
	// done later..

	// minimum stemming
	vector<string>::iterator index = tokens.begin();
	string extr; // this is substr
	string mat = "\'s";	// this is to be removed from token
	string::size_type posx;
	size_t len =0;

	while(index != tokens.end())
	{
		// *index contains string..
		len = index->length();
		if( len > 3)
		{
			//cout << "token is" << *index << endl;
			extr.clear();
			posx =0;
			posx = len - 2;
			extr = index->substr(posx, 2);
			if (extr == mat)
				index->erase(posx, 2);
		}
		index++;
	}


	// remove one char tokens...
	index = tokens.begin();
	vector<string>::iterator end = tokens.end();
	while(index != end)
	{
		if (index->length() == 1)
		{
			index = tokens.erase(index);
			end = tokens.end();
		}
		else{	
			index++;
		}
	} // while


	// remove digit (number) tokens...
	index = tokens.begin();
	end = tokens.end();
	while(index != end)
	{
		if (isnumber(*index))
		{
			index = tokens.erase(index);
			end = tokens.end();
		}
		else{	
			index++;
		}
	} // while
}
// TOKENIZE ENDS HERE



void getDelim(string &str)
{
	unsigned char c = 0;
	
	while(c < 255)
	{
		if(!isalpha(c)) {
			str.push_back(c);
		}
		c++;
	}
}


bool checkalpha(char ch)
{
	if( ((ch >= 65) && (ch <= 90)) || ((ch >= 97) && (ch <= 122)) )
		return true;
	else
		return false;
}

bool isnumber(string str)
{
	string::iterator itr;
	itr = str.begin();
	while(itr != str.end())
	{
		if(isdigit(*itr))
			itr++;
		else 
			return false;
	}

	return true;
}

// make list of stop words
void getStopwords(string filename)
{
	// open file ..
	ifstream ifs;
	ifs.open(filename.c_str());
	
	string stop;
	stop.clear();
			
	char ch;
	while(!ifs.eof())
	{
		ifs.get(ch);

		// stdlib isalpha causes assert and not used.
		if(checkalpha(ch))
		{
			stop.push_back(ch);
		}		
		else if(ch == '\n') {
			std::transform(stop.begin(), stop.end(), stop.begin(), ::tolower);
			cout << stop << endl;
			stopwords.push_back(stop);
			stop.clear();
		}
	} // while
}


void GetDatasetTokens(string datasetpath, vector<string> &dstokens)
{
	vector<string> dataset_tokens;
	// for enumeration of all files in the directory..
	CListDir listDataset;
	listDataset.SetDirectoryPath(datasetpath);
	listDataset.listDir("");


	cout << "getdatasettokens: " << datasetpath << endl;
	Sleep(5);

	// filenames member of CListDir is public and contains file names..
	vector<string>::iterator itr = listDataset.filenames.begin();
	string data;
	
	vector<string> tokens;

	int fc = 1;
	while(itr != listDataset.filenames.end())
	{
		cout << "reading :" << fc  << endl; //<< *itr << endl;
		data.clear();
		tokens.clear();

		// read into data from <TEXT> of XML FILE
		xmlRead(*itr, data);
		
		tokens.clear();

		// now tokenize
		Tokenize(data, tokens, delim);
		//split(tokens, data);

		dataset_tokens.insert(dataset_tokens.begin(), tokens.begin(), tokens.end());	
		
		// iterate to next file
		itr++;
		fc++; // just to show progress, how many files have been completed..
	}

	// convert all tokens to lower case characters
	vector<string>::iterator itr_low = dataset_tokens.begin();
	while(itr_low != dataset_tokens.end())
	{
		std::transform( (*itr_low).begin(), (*itr_low).end(), (*itr_low).begin(), ::tolower);
		itr_low++;
	}


	// stemming
	vector<string>::iterator itr_stm = dataset_tokens.begin();
	while(itr_stm != dataset_tokens.end())
	{
		std::wstring tempw(itr_stm->begin(), itr_stm->end());

		stemming::english_stem<> StemEnglish;
		StemEnglish(tempw);

		std::string temp(tempw.begin(), tempw.end());
		dstokens.push_back(temp);
		itr_stm++;
	}

}

void dostemming(vector<string> &input)
{
	vector<string> tokens ;
	tokens.insert(tokens.begin(), input.begin(), input.end());

	input.clear();

	// stemming
	vector<string>::iterator itr_stm = tokens.begin();
	while(itr_stm != tokens.end())
	{
		std::wstring tempw(itr_stm->begin(), itr_stm->end());

		stemming::english_stem<> StemEnglish;
		StemEnglish(tempw);

		std::string temp(tempw.begin(), tempw.end());
		input.push_back(temp);
		itr_stm++;
	}
}


void GetDFMap(vector<string> datasets, map<string,int> &dfmap, unsigned int &maxd)
{
	// This contains all the file names in class1 and class2
	vector<string> collection;

	vector<string>::iterator ds_itr;

	ds_itr = datasets.begin();
	while(ds_itr != datasets.end())
	{	
		// for enumeration of all files in the directory..
		CListDir listDataset;
		listDataset.SetDirectoryPath(*ds_itr);
		listDataset.listDir("");

		collection.insert(collection.end(), listDataset.filenames.begin(), listDataset.filenames.end());
		ds_itr++;
	}

	// Alert : dfmap is already filled with dict items
	
	map<string,int>::iterator itr_dfmap = dfmap.begin();
	while(itr_dfmap != dfmap.end())
	{
		itr_dfmap->second = 0;
		itr_dfmap++;
	}

	// we have to find in how many documents the term occurs
	unsigned int index = 0;
	vector<string> tokens;
	vector<string>::iterator itr_t;

	vector<string>::iterator itr = collection.begin();
	string data;

	vector<string>::iterator itr_low ;

	while(itr != collection.end())
	{
		cout << "reading :" << index <<endl; //<< *itr << endl;
		data.clear();
		tokens.clear();

		// read into data from <TEXT> of XML FILE
		xmlRead(*itr, data);
		
		tokens.clear();

		// now tokenize
		Tokenize(data, tokens, delim);
		//split(tokens, data);

		// convert all tokens to lower case characters
		itr_low = tokens.begin();
		while(itr_low != tokens.end())
		{
			std::transform( (*itr_low).begin(), (*itr_low).end(), (*itr_low).begin(), ::tolower);
			itr_low++;
		}

		dostemming(tokens);
		map<string,int>::iterator dftemp;

		itr_t = tokens.begin();
		// dfmap doesnot contain stop words..
		while(itr_t != tokens.end())
		{
			dftemp = dfmap.find(*itr_t);
			if(dftemp != dfmap.end())
				dftemp->second += 1;
			itr_t++;
		}

		// iterate to next file
		itr++;
		index++; // just to show progress, how many files have been completed..
	}
	
	maxd = collection.size();
}


void  GetMapFromTokens(vector<string> &dataset_tokens, map<string,int> &tfmap)
{
	// create tfmap
	vector<string>::iterator itrv = dataset_tokens.begin();
	map<string,int>::iterator itrmap = tfmap.begin();
	map<string,int>::iterator tempmap;
	
	// construct a map from our tokens
	while(itrv != dataset_tokens.end())
	{
		tempmap = tfmap.find(*itrv);
		if (tempmap == tfmap.end())
			tfmap.insert(pair<string, int>(*itrv, 1));
		else
			tempmap->second += 1;

		itrv++;
	}

	// remove stop words from our token set
	vector<string>::iterator stopitr = stopwords.begin();
	map<string,int>::iterator tm;
	while(stopitr != stopwords.end())
	{
		tm = tfmap.find(*stopitr);

		if(tm != tfmap.end())
			tfmap.erase(tm);

		stopitr++;
	}
}

