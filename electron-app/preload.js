const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('electronAPI', {
  sendFileToMainProcess: (x,filePath) => {
    ipcRenderer.send('file-selected', { x, filePath } );
  },
  receiveProcessedFilePath: (callback) => {
    ipcRenderer.on('human-readable successed', (event, n) => {
      console.log('Preload - Nr pachete ',n);
      callback(n);
    });
  },
});
