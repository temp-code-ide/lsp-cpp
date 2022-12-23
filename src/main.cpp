#include <iostream>
#include <thread>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include "client.h"

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

//{"id":"textDocument/documentSymbol", "jsonrpc" : "2.0", "result" : [{"children": [{"detail":"void ()", "kind" : 6, "name" : "SayHello", "range" : {"end":{"character":22, "line" : 8}, "start" : {"character":4, "line" : 8}}, "selectionRange" : {"end":{"character":17, "line" : 8}, "start" : {"character":9, "line" : 8}}}, { "detail":"std::string","kind" : 8,"name" : "name","range" : {"end":{"character":20,"line" : 10},"start" : {"character":4,"line" : 10}},"selectionRange" : {"end":{"character":20,"line" : 10},"start" : {"character":16,"line" : 10}} }, { "detail":"int","kind" : 8,"name" : "age","range" : {"end":{"character":11,"line" : 11},"start" : {"character":4,"line" : 11}},"selectionRange" : {"end":{"character":11,"line" : 11},"start" : {"character":8,"line" : 11}} }, { "detail":"bool","kind" : 8,"name" : "sex","range" : {"end":{"character":12,"line" : 12},"start" : {"character":4,"line" : 12}},"selectionRange" : {"end":{"character":12,"line" : 12},"start" : {"character":9,"line" : 12}} }] , "detail" : "class", "kind" : 5, "name" : "CStudent", "range" : {"end":{"character":1, "line" : 13}, "start" : {"character":0, "line" : 5}}, "selectionRange" : {"end":{"character":14, "line" : 5}, "start" : {"character":6, "line" : 5}}}, { "detail":"void ()","kind" : 12,"name" : "test","range" : {"end":{"character":1,"line" : 16},"start" : {"character":0,"line" : 14}},"selectionRange" : {"end":{"character":9,"line" : 14},"start" : {"character":5,"line" : 14}} }, { "detail":"int ()","kind" : 12,"name" : "main","range" : {"end":{"character":1,"line" : 30},"start" : {"character":0,"line" : 17}},"selectionRange" : {"end":{"character":8,"line" : 17},"start" : {"character":4,"line" : 17}} }] }

int main() {
       
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
            p.line = 20;
            p.character = 7;
            ctx.triggerCharacter = "say";

            RequestID id = client.Completion(file, p, ctx);
            my.bindResponse(id, [&](value& v)->void {
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
