const humanReadable = document.querySelector('#human_readable');
const consolaTextarea = document.querySelector('#consola');
const fileObj=document.querySelector('#file');
const consola=document.querySelector('#consola');

let file;

function loadFile(e){
  file=e.target.files[0];
  if (file.name.endsWith('.pcap')){
    const eroareCell = document.getElementById('eroare');
    eroareCell.textContent = '';
    //salveaza fisierul local si ii ia calea
    //saveFileLocally(file);

    humanReadable.disabled = false;
  }
  else{
    const eroareCell = document.getElementById('eroare');
    eroareCell.textContent = 'Your file is not in PCAP format. Load a PCAP file.';
    humanReadable.disabled = true;
  }
}

function humanFct(e) {
  if (file) {
    // Trimiterea fișierului către procesul principal
    //console.log(file);
    window.electronAPI.sendFileToMainProcess('HR',file.name);
  
    // Ascultarea răspunsului de la procesul principal
    /*window.electronAPI.receiveProcessedFilePath((filePath) => {
      console.log('Calea absolută a fișierului procesat:', filePath);
    });*/
  }
}

//asculta raspunsul de la procesul principal
/*ipcRenderer.on('file-processed', (event, filePath) => {
  console.log('Calea absolută a fișierului procesat:', filePath);
});*/

fileObj.addEventListener('change',loadFile);

humanReadable.addEventListener('click', humanFct);