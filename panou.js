//marimea adaptabila a componentelor in functie de ecran
let screenWidth = window.innerWidth;
const consola = document.querySelector('#consola');
const colect = document.querySelector('#colect');
const filtreID=document.querySelector('#filtreID');

consola.style.width = screenWidth - 90 + 'px';
colect.style.width = screenWidth - 90 + 'px';
filtreID.style.width = screenWidth - 90 + 'px';

window.addEventListener('resize', function() {
  screenWidth=window.innerWidth;
  consola.style.width = screenWidth - 90 + 'px';
  colect.style.width = screenWidth - 90 + 'px';
  filtreID.style.width = screenWidth - 90 + 'px';});

var save = document.getElementById("save");
save.addEventListener("click", function() {
  var fileInput = document.getElementById('file');
  verifica(fileInput.files[0]);

  //verificam la schimbarea fisierului daca e .pcap
  fileInput.addEventListener('change', () => {
    verifica(fileInput.files[0]);
  });
});

function activeaza() {
    document.getElementById("human_readable").disabled = false;
    document.getElementById("hex").disabled = false;
    document.getElementById("capture").disabled = false;}

function dezactiveaza() {
    document.getElementById("human_readable").disabled = true;
    document.getElementById("hex").disabled = true;
    document.getElementById("capture").disabled = true;}

function verifica(file) {
  if (!(file.name.endsWith('.pcap'))){
    const eroareCell = document.getElementById('eroare');
    eroareCell.textContent = 'Your file is not in PCAP format. Load a PCAP file.';
    dezactiveaza();
  }else{
    const eroareCell = document.getElementById('eroare');
    eroareCell.textContent = '';
    activeaza();
}
}