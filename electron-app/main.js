const { app, BrowserWindow, Menu, ipcMain, shell } = require('electron');
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

  //inchide fereastra cand utilizatorul o inchide
  //mainWindow.on('closed', function() {mainWindow = null;});

  ipcMain.on('file-selected', (event, { x,file} ) => {
    //console.log(file,' - ',x);
    //console.log(app.getPath('userData'));
    const filePath = path.join(app.getPath('userData'), file);

    //butonul human readable
    if(x === 'HR'){
      var executable='./PcapRead.exe';
      var runner=cp.execFile(executable,[filePath],(error,out,err)=>{
        if (error) {
          throw error;
        }else{
              console.log(out);
          }
        });
      }
    //event.reply('file-processed', filePath);
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