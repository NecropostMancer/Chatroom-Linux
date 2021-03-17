//主要是后端脚本.
const electron = require("electron");
const app = electron.app;
const BrowserWindow = electron.BrowserWindow;

//开发热加载。
try {
    require('electron-reloader')(module,{});
}catch(_){}

function createWindow(){
    //electron.Menu.setApplicationMenu(null);
    const win = new BrowserWindow(
        {
            width: 800,
            height: 600,
            //resizable: false,
            webPreferences:{
                nodeIntegration: true,
                enableRemoteModule: true
            },
            
        }
    )

    win.loadFile('./src/login.html')
}


app.whenReady().then(createWindow);


app.on('activate', ()=>{
    if(BrowserWindow.getAllWindows.length === 0){
        createWindow()
    }
})


