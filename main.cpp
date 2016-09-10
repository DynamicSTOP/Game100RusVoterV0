#include "QApplication"
#include "QtCore"
#include "wincotroller.h"

int main(int argc,char** argv){
    QApplication *app = new QApplication(argc,argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
    WinCotroller *w = new WinCotroller();
    w->show();
    app->exec();
    return 0;
}




/* getting proxys


var listtable = $("#listtable");
var rows = listtable.children("tbody").children("tr");

var list=[];

for(var r in rows){
    var rowId = parseInt(r);
    if(isNaN(rowId))
        continue;

    if(!rows.hasOwnProperty(r))
        continue;
    var tdList = $(rows[r]).children("td");

    var tdIp=tdList[1];


    var ipStr = $(tdIp).html();
    ipStr = ipStr.replace("div","span");
    var style = ipStr.substring(ipStr.indexOf("<style>"),ipStr.indexOf("</style>"));
    Parts = style.split(".");
    Bad = [];
    for(var i in Parts){
        var partsId = parseInt(i);
        if(isNaN(partsId))
            continue;
        if(!Parts.hasOwnProperty(i))
            continue;
        if(Parts[i].indexOf(":none")!=-1){
            Bad.push(Parts[i].substring(0,4));
        }
    }

    ipStr = ipStr.replace("<span>"+style+"</style>","");
    while(ipStr.indexOf("div")!=-1)
        ipStr = ipStr.replace("div","span");

    var Spans = ipStr.match(/<span[^.^>]*>[\.\d]*<\/span>/mg);
    for(var i in Spans){
        var partsId = parseInt(i);
        if(isNaN(partsId))
            continue;
        if(!Spans.hasOwnProperty(i))
            continue;
        if(Spans[i].indexOf(":none")!=-1){
            ipStr=ipStr.replace(Spans[i],"");
        } else {
            for(var b in Bad){
                var partsId = parseInt(b);
                if(isNaN(partsId))
                    continue;
                if(!Bad.hasOwnProperty(b))
                    continue;
                if(Spans[i].indexOf(Bad[b])!=-1){
                    ipStr=ipStr.replace(Spans[i],"");
                }
            }
        }
    }

    ipStr = ipStr.replace(/(<span[^.^>]*>)/g,'');
    ipStr = ipStr.replace(/(<\/span>)/g,'');

    var tdPort=$(tdList[2]).html();
    if(tdPort!=null && tdPort.length>0)
        tdPort=tdPort.replace(/\n/g,"");

    var proxy = ipStr+":"+tdPort+":"+$(tdList[6]).html();

    list.push(proxy);
}
console.log(list.join('\n'));

*/
