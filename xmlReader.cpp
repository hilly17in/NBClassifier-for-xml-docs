#include "stdafx.h"
 
HRESULT WriteAttributes(IXmlReader* pReader) 
{ 
    const WCHAR* pwszPrefix; 
    const WCHAR* pwszLocalName; 
    const WCHAR* pwszValue; 
    HRESULT hr = pReader->MoveToFirstAttribute(); 
 
    if (S_FALSE == hr) 
        return hr; 
    if (S_OK != hr) 
    { 
        wprintf(L"Error moving to first attribute, error is %08.8lx", hr); 
        return hr; 
    } 
    else 
    { 
        while (TRUE) 
        { 
            if (!pReader->IsDefault()) 
            { 
                UINT cwchPrefix; 
                if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix))) 
                { 
                    wprintf(L"Error getting prefix, error is %08.8lx", hr); 
                    return hr; 
                } 
                if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) 
                { 
                    wprintf(L"Error getting local name, error is %08.8lx", hr); 
                    return hr; 
                } 
                if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) 
                { 
                    wprintf(L"Error getting value, error is %08.8lx", hr); 
                    return hr; 
                } 
                if (cwchPrefix > 0) 
                    wprintf(L"Attr: %s:%s=\"%s\" \n", pwszPrefix, pwszLocalName, pwszValue); 
                else 
                    wprintf(L"Attr: %s=\"%s\" \n", pwszLocalName, pwszValue); 
            } 
 
            if (S_OK != pReader->MoveToNextAttribute()) 
                break; 
        } 
    } 
    return hr; 
} 




unsigned long
xmlRead(string xmlFile, string &data)
{
	HRESULT hr = S_OK; 
    IStream *pFileStream = NULL; 
    IXmlReader *pReader = NULL; 
    XmlNodeType nodeType; 
    const WCHAR* pwszPrefix; 
    const WCHAR* pwszLocalName; 
    const WCHAR* pwszValue; 
    UINT cwchPrefix; 
	int flag=0;



    //Open read-only input stream 
	if (FAILED(hr = SHCreateStreamOnFile(xmlFile.c_str(), STGM_READ, &pFileStream))) 
    { 
        wprintf(L"Error creating file reader, error is %08.8lx", hr); 
        HR(hr); 
    } 
 
    if (FAILED(hr = CreateXmlReader(__uuidof(IXmlReader), (void**) &pReader, NULL))) 
    { 
        wprintf(L"Error creating xml reader, error is %08.8lx", hr); 
        HR(hr); 
    } 
 
    if (FAILED(hr = pReader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit))) 
    { 
        wprintf(L"Error setting XmlReaderProperty_DtdProcessing, error is %08.8lx", hr); 
        HR(hr); 
    } 
 
    if (FAILED(hr = pReader->SetInput(pFileStream))) 
    { 
        wprintf(L"Error setting input for reader, error is %08.8lx", hr); 
        HR(hr); 
    } 
 
    //read until there are no more nodes 
    while (S_OK == (hr = pReader->Read(&nodeType))) 
    { 
        switch (nodeType) 
        { 
        case XmlNodeType_XmlDeclaration: 
            wprintf(L"XmlDeclaration\n"); 
            if (FAILED(hr = WriteAttributes(pReader))) 
            { 
                wprintf(L"Error writing attributes, error is %08.8lx", hr); 
                HR(hr); 
            } 
            break; 
        case XmlNodeType_Element: 
            if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix))) 
            { 
                wprintf(L"Error getting prefix, error is %08.8lx", hr); 
                HR(hr); 
            } 
            if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) 
            { 
                wprintf(L"Error getting local name, error is %08.8lx", hr); 
                HR(hr); 
            } 
            if (cwchPrefix > 0) 
                wprintf(L"Element: %s:%s\n", pwszPrefix, pwszLocalName); 
            else 
                wprintf(L"Element Local: %s\n", pwszLocalName);  
 
			if(!wcscmp(pwszLocalName, L"TEXT"))
				flag = 1;


            if (FAILED(hr = WriteAttributes(pReader))) 
            { 
                wprintf(L"Error writing attributes, error is %08.8lx", hr); 
                HR(hr); 
            } 
 
            if (pReader->IsEmptyElement() ) 
                wprintf(L" (empty)"); 
            break; 

        case XmlNodeType_EndElement: 
            if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix))) 
            { 
                wprintf(L"Error getting prefix, error is %08.8lx", hr); 
                HR(hr); 
            } 
            if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) 
            { 
                wprintf(L"Error getting local name, error is %08.8lx", hr); 
                HR(hr); 
            } 
            if (cwchPrefix > 0) 
                wprintf(L"End Element: %s:%s\n", pwszPrefix, pwszLocalName); 
            else 
                wprintf(L"End Element: %s\n", pwszLocalName); 
            break; 
        case XmlNodeType_Text: 
        case XmlNodeType_Whitespace: 
            if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) 
            { 
                wprintf(L"Error getting value, error is %08.8lx", hr); 
                HR(hr); 
            } 

			if (flag == 1)
			{
				int nlen=0;
				char *pszValue;
				//wprintf(L"Found Text: >%s<\n", pwszValue); 
				nlen = WideCharToMultiByte( CP_ACP, // ANSI Code Page
							0, // No special handling of unmapped chars
							pwszValue, // wide-character string to be converted
							wcslen(pwszValue),
							NULL, 0, // No output buffer since we are calculating length
							NULL, NULL ); // Unrepresented char replacement - Use Default 

				pszValue = new char[ nlen ]; // nUserNameLen includes the NULL character

				WideCharToMultiByte( CP_ACP, // ANSI Code Page
							0, // No special handling of unmapped chars
							pwszValue, // wide-character string to be converted
							wcslen(pwszValue),
							pszValue, nlen, // No output buffer since we are calculating length
							NULL, NULL ); // Unrepresented char replacement - Use Default 
			
				printf("Found Text %d: ><\n", nlen); 
				data.clear();
				std::string str(pszValue);
				data = str;
				flag =0;
			}
			

            break; 
        case XmlNodeType_CDATA: 
            if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) 
            { 
                wprintf(L"Error getting value, error is %08.8lx", hr); 
                HR(hr); 
            } 
            wprintf(L"CDATA: %s\n", pwszValue); 
            break; 
        case XmlNodeType_ProcessingInstruction: 
            if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) 
            { 
                wprintf(L"Error getting name, error is %08.8lx", hr); 
                HR(hr); 
            } 
            if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) 
            { 
                wprintf(L"Error getting value, error is %08.8lx", hr); 
                HR(hr); 
            } 
            wprintf(L"Processing Instruction name:%s value:%s\n", pwszLocalName, pwszValue); 
            break; 
        case XmlNodeType_Comment: 
            if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) 
            { 
                wprintf(L"Error getting value, error is %08.8lx", hr); 
                HR(hr); 
            } 
            wprintf(L"Comment: %s\n", pwszValue); 
            break; 
        case XmlNodeType_DocumentType: 
            wprintf(L"DOCTYPE is not printed\n"); 
            break; 
        } 
    } 
 
CleanUp: 
    SAFE_RELEASE(pFileStream); 
    SAFE_RELEASE(pReader); 
    return hr; 	

}