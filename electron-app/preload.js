const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('electronAPI', {
  sendFileToMainProcess: (x,file) => {
    //console.log("am ajuns aici", file);
    ipcRenderer.send('file-selected', { x, file } );
  },
  receiveProcessedFilePath: (callback) => {
    ipcRenderer.on('file-processed', (event, filePath) => {
      callback(filePath);
    });
  },
});