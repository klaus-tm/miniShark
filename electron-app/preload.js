const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('electronAPI', {
  sendFileToMainProcess: (file) => {
    console.log("am ajuns aici", file);
    ipcRenderer.send('file-selected', file);
  },
  receiveProcessedFilePath: (callback) => {
    ipcRenderer.on('file-processed', (event, filePath) => {
      callback(filePath);
    });
  },
});
