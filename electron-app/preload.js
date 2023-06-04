const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('electronAPI', {
  sendFileToMainProcess: (x,filePath) => {
    //console.log("Preload - ", filePath);
    ipcRenderer.send('file-selected', { x, filePath } );
  },
  receiveProcessedFilePath: (callback) => {
    ipcRenderer.on('human-readable successed', (event, filePath) => {
      callback(filePath);
    });
  },
});
