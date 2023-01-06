#include <iostream>
#include <thread>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include "client.h"
#include <stdio.h>
#include "json.hpp";
using json = nlohmann::json;


std::string readFile(std::string file)
{
    std::fstream fs(file, std::ios_base::out | std::ios_base::in);   
    std::ostringstream  oss;
    oss << fs.rdbuf();
    std::string text = oss.str();
    fs.close();
    return text;
}

void writeFile(const std::string &file, std::string text)
{
    std::ofstream ofs;
    ofs.open(file, std::ios::out);
    ofs << text << std::endl;
    ofs.close();   
}

int main() {
    
    std::ifstream f("d:/hello_test/lsp.json");
    json data = json::parse(f);
    std::vector<CompletionItem> vec = data["items"].get<std::vector<CompletionItem>>();

    ProcessLanguageClient client("D:\\clangd.exe");
    MapMessageHandler my;
    std::thread thread([&] {
        client.loop(my);
    });
    string_ref rootUri = "D:/hello_test/";
    string_ref file = "file:///D:/hello_test/main.cpp";
    std::string text = readFile("d:/hello_test/main.cpp");

    string_ref file2 = "file:///D:/hello_test/hello.h";
    std::string text2 = readFile("d:/hello_test/hello.h");


    string_ref file3 = "file:///D:/hello_test/hello.cpp";
    std::string text3 = readFile("d:/hello_test/hello.cpp");

    std::string jsonFile = "d:/hello_test/lsp.json";

    int res;
    while (scanf("%d", &res)) {
        if (res == 1) {
            client.Exit();
            thread.detach();
            return 0;
        } else if (res == 2) {

            client.Initialize(rootUri);         
           // client.Initialize();
        }
        else if (res == 22) {
            client.WorkspaceSymbol("D:/hello_test");
        }
        else if (res == 3) {
            client.DidOpen(file, text );
            //client.DidOpen(file2, text2);
            //client.DidOpen(file3, text3);
            //client.DidOpen(file2, text2);
             client.Sync();
        } else if (res == 4) {
            json j;         
            RequestID id = client.Formatting(file);
      //      client.readJson(j);
        }
        else if (res == 5) {
            json j;
            Position p;
            p.line = 30;
            p.character = 6;
            RequestID id = client.GoToDefinition(file, p); 

        //    client.readJson(j);
        }
        else if (res == 6)
        {
            Position p;
            p.line = 29;
            p.character = 6;
            RequestID id = client.DocumentHighlight(file, p);
        }
        else if (res == 7)
        {
            RequestID id = client.DocumentColor(file);
        }
        else if (res == 8)
        {

            RequestID id = client.DocumentSymbol(file);
            my.bindResponse(id, [](value &v)->void {
                std::cout << "[##]" << v << std::endl;
                
                auto j = v;      
                auto k = v["children"];
                auto list = j.get<std::list<DocumentSymbol>>();
                for (auto x : list)
                {
                    std::cout << x.name << std::endl;
                }

         
                auto klist = k.get<std::list<DocumentSymbol>>();
                for (auto x : klist)
                {
                    std::cout << x.name << std::endl;
                }

            });
        }
        else if (res == 9)
        {
            RequestID id = client.WorkspaceSymbol(rootUri);
            
        }
        else if (res == 10)
        {
            CompletionContext ctx;
            Position p;
            p.line = 23;
            p.character = 6;
            ctx.triggerCharacter = "he";

            RequestID id = client.Completion(file, p, ctx);
            my.bindResponse(id, [&](value& v)->void {
                std::vector<CompletionItem> vec = v["items"].get<std::vector<CompletionItem>>();
                writeFile(jsonFile, v.dump());
            });
        }
        else if (res == 11)
        {
            Position p;
            p.line = 19;
            p.character = 7;

            RequestID id = client.Hover(file, p);
            my.bindResponse(id, [&](value& v)->void {
                writeFile(jsonFile, v.dump());
                });
        }
        else if (res == 12)
        {
            Position p;
            p.line = 21;
            p.character = 17;

            RequestID id = client.Rename(file, p, "lisi");
            my.bindResponse(id, [&](value& v)->void {
                writeFile(jsonFile, v.dump());
                });
        }
        else if (res == 13)
        {
            Position p;
            p.line = 19;
            p.character = 7;

            RequestID id = client.References(file, p);
            my.bindResponse(id, [&](value& v)->void {
                writeFile(jsonFile, v.dump());
                });
        }
        else if (res == 14)
        {
            Position p;
            p.line = 21;
            p.character = 7;
            RequestID id = client.TypeHierarchy(file, p, TypeHierarchyDirection::Both, 0);
            my.bindResponse(id, [&](value& v)->void {
                writeFile(jsonFile, v.dump());
                });
        }

    }
    return 0;
}
