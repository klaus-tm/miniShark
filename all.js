//trecerea light-dark mode
const checkbox = document.getElementById("checkbox");
checkbox.addEventListener("change", function() {
  document.body.classList.toggle("dark");
  
  // Salvează starea în localStorage
  if (document.body.classList.contains("dark")) {
    localStorage.setItem("mode", "dark");
  } else {
    localStorage.setItem("mode", "light");
  } });

// Încarcă starea din localStorage la încărcarea paginii
const savedMode = localStorage.getItem("mode");
if (savedMode === "dark") {document.body.classList.add("dark");}
else {document.body.classList.remove("dark"); }



//marimea adaptabila a componentelor in functie de ecran
let screenWidth_all = window.innerWidth;
const header = document.querySelector('#header');
header.style.width = screenWidth_all - 60 + 'px';

//redimensionarea componentelor daca ecranul isi schmba dimensiunea
window.addEventListener('resize', function() {
  screenWidth_all=window.innerWidth;
  header.style.width = screenWidth_all - 60 + 'px';});



const logo = document.getElementById('logo');
const img = new Image();
img.onload = function() {
  const width = img.width;
  const height = img.height;
  logo.style.width = width/5 + 'px';
  logo.style.height = height/5 + 'px';
}
img.src = 'style/logo-light.png';