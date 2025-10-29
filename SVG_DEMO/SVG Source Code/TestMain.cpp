#include "stdafx.h"
#include "Library.h"
#include <Windows.h>
#include <cstdio>   

void PrintRawXML(xml_node<>* node, int indent) {
    if (!node) return;
    string indentation(indent * 2, ' ');
    cout << indentation << "<" << node->name();

    for (auto* attr = node->first_attribute(); attr != nullptr; attr = attr->next_attribute()) {
        cout << " " << attr->name() << "=\"" << attr->value() << "\"";
    }

    xml_node<>* child = node->first_node();

    if (!child) {
        cout << " />\n";
    }
    else {
        std::cout << ">\n";
        while (child) {
            PrintRawXML(child, indent + 1);
            child = child->next_sibling();
        }

        cout << indentation << "</" << node->name() << ">\n";
    }
}


//Main IS HERE
int AppLogic() {
    /*cout << "SVG Demo Start" << endl;

    SVGRoot svgRoot;
    svgRoot.loadFromFile("sample.svg");
    svgRoot.render();

    cout << "SVG Demo End\n\n";*/


    // Test: Print loaded elements

    ifstream file("sample.svg");
    if (!file.is_open()) {
        cout << "Error: Khong the mo file sample.svg" << endl;
        return -1;
    }

    xml_document<> sourceFile;
    vector<char> buffer((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    buffer.push_back('\0');
    sourceFile.parse<0>(reinterpret_cast<char*>(buffer.data()));

    xml_node<>* rootNode = sourceFile.first_node();
    PrintRawXML(rootNode, 0);
    file.close();

    cout << "\n\nNhan Enter de ket thuc..." << endl;
    cin.get();

    return 0;
}

// To create a console window for a GUI application
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {


    AllocConsole();


    FILE* pCout, * pCerr, * pCin;
    freopen_s(&pCout, "CONOUT$", "w", stdout); 
    freopen_s(&pCerr, "CONOUT$", "w", stderr); 
    freopen_s(&pCin, "CONIN$", "r", stdin);  

 
    cout.sync_with_stdio();
    cin.sync_with_stdio();

    
    int result = AppLogic();

  
    if (pCout) fclose(pCout);
    if (pCerr) fclose(pCerr);
    if (pCin) fclose(pCin);
    FreeConsole();

    return result;
}