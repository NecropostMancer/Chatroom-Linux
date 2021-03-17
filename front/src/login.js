const { remote } = require('electron')

document.querySelector('#login-btn').addEventListener("click",login);
document.querySelector('#reg-btn').addEventListener("click",register);
document.querySelector('#reg').addEventListener("click",OpenReg);
var loginPage = document.querySelector('#login-page');
var regPage = document.querySelector('#reg-page');

var usernameElement = document.querySelector('#username');
var passwordElement = document.querySelector('#password');

var regUsernameElement = document.querySelector('#username-new');
var regPasswordElement = document.querySelector('#password-new');
var vaildElement = document.querySelector('#password-new-re');

function OpenLogin(){
    regPage.classList.add("hide");
    loginPage.classList.remove("hide");
}

function OpenReg(){
    loginPage.classList.add("hide");
    regPage.classList.remove("hide");
}

console.log(remote)

const {LoginRequest ,RegisterRequest} = require('./request.js')

function login(){
    let username = usernameElement.value;
    let password = passwordElement.value;
    LoginRequest(username,password).then(()=>{
        remote.getCurrentWindow().setSize(1000,1000);
        remote.getCurrentWindow().loadFile('./src/index.html')
    }).catch((e)=>{
        console.log(e);
    })
    
}

function register(){
    let username = regUsernameElement.value;
    let password = regPasswordElement.value;
    RegisterRequest(username,password,"ChooseYourFighter!").then(()=>{
        OpenLogin();
    }).catch((e)=>{
        console.log(e);
        OpenLogin();
    })
}