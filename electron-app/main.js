const { app, BrowserWindow, Menu, ipcMain, shell, dialog } = require('electron');
const fs = require('fs');
const path = require('path');
const url = require('url');
const cp = require('child_process');

//let mainWindow;

function createWindow() {
  //creeaza o fereastra de browser cu o dimensiune initiala de 800x600 pixeli
  mainWindow = new BrowserWindow({
    width: 1000,
    height: 800,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
    },
  });


  //incarca fisierul HTML "home.html" in fereastra
  mainWindow.loadURL(
    url.format({
      pathname: path.join(__dirname, '../frontend/home.html'),
      protocol: 'file:',
      slashes: true
    })
  );

  ipcMain.on('file-selected', (event, { x, filePath } ) => {
    //console.log('Main - ',x,filePath);
    //butonul human readable
    if(x === 'HR'){
      var executable='./PcapRead.exe';
      var runner=cp.execFile(executable,[filePath],(error,out,err)=>{
        if (error) {
          console.error('Error:', error);
          console.error('Stderr:', err);
          throw error;
        }else{
              let n='';
              for(let i in out){
                let value = out[i];
                if (value !== ' ') n += value;
                else break;
              }
              console.log('Main - Nr pachete: ',n);
              event.reply('human-readable successed', n);
          }
        });
      }
  

  });
}



//creeaza fereastra cand Electron este gata
app.on('ready', () => {
  createWindow();

  // Remove variable from memory
  mainWindow.on('closed', () => (mainWindow = null));
});
 
//inchide aplicatia cand toate ferestrele sunt inchise (cu exceptia aplicatiilor MacOS)
app.on('window-all-closed', function() {
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

//creeaza fereastra cand aplicatia este activata si nu exista alte ferestre deschise
app.on('activate', function() {
  if (mainWindow === null) {
    createWindow();
  }
});