//以下主要是前端脚本。
//自动调节发送信息框的大小。
var autoTextarea = function (elem, extra, maxHeight) {
    extra = extra || 0;
    var addEvent = function (type, callback) {
        elem.addEventListener(type, callback, false)
    },
    getStyle = function (name) {
        return getComputedStyle(elem, null)[name];
    },   
    minHeight = parseFloat(getStyle('height'));
    elem.style.resize = 'none';
    var change = function () {
        var scrollTop, height,
        padding = 0,
        style = elem.style;
        if (elem._length === elem.value.length) return;
        elem._length = elem.value.length;
        padding = parseInt(getStyle('paddingTop')) + parseInt(getStyle('paddingBottom'));
        scrollTop = document.body.scrollTop || document.documentElement.scrollTop;
        elem.style.height = minHeight + 'px';
        if (elem.scrollHeight > minHeight) {
        if (maxHeight && elem.scrollHeight > maxHeight) {
            height = maxHeight - padding;
            style.overflowY = 'auto';
        } else {
            height = elem.scrollHeight - padding;
            style.overflowY = 'hidden';
        };
            style.height = height + extra + 'px';
            scrollTop += parseInt(style.height) - elem.currHeight;
            document.body.scrollTop = scrollTop;
            document.documentElement.scrollTop = scrollTop;
            elem.currHeight = parseInt(style.height);
        };
    };
    addEvent('propertychange', change);
    addEvent('input', change);
    addEvent('focus', change);
    change();
    };

var text = document.getElementById("text");
autoTextarea(text);

String.prototype.formatUnicorn = String.prototype.formatUnicorn ||
function () {
    "use strict";
    var str = this.toString();
    if (arguments.length) {
        var t = typeof arguments[0];
        var key;
        var args = ("string" === t || "number" === t) ?
            Array.prototype.slice.call(arguments)
            : arguments[0];

        for (key in args) {
            str = str.replace(new RegExp("\\{" + key + "\\}", "gi"), args[key]);
        }
    }

    return str;
};

//模板引擎真好用，先自己做一个丐版的算了


function htmlToElement(html) {
    var template = document.createElement('template');
    html = html.trim(); // Never return a text node of whitespace as the result
    html = html.formatUnicorn(...Array.prototype.slice.call(arguments,1));
    template.innerHTML = html;
    var element = template.content.firstChild;
    element.remove();
    return element;
}

var roomItem = `
<div class = "room-item">
    <span class = "font-bolder">{0}</span>
</div>
`

var msgItem = `
<div class = "msg-item">
    <span class = "font-bolder username">{0}</span>
    <span class = "font-normal message">{1}</span>
</div>
` 

var noticeItem = `
<div class = "notice-item">
    <span class="font-notice">{0}</span>
</div>
`

var container = `
<div class = "vert-container">
</div>
`

const classRoomSelected = 'selected';
const classRoomNotice = 'notice';

function getRoomItem(str){
    let element = htmlToElement(roomItem,arguments);
    element.addEventListener("click",(e)=>{SwitchRoom(e)});
    return element;
}

function getMsgItem(str){
    return htmlToElement(msgItem,arguments);
}

function getNoticeItem(str){
    return htmlToElement(noticeItem,arguments);
}


//测试用
function populate(){
    let roomRoot = document.querySelector('#room-pan  .vert-container');
    let msgRoot = document.querySelector('#chat-pan .vert-container');
    console.log("calc");
    for(let i=0;i<4;i++){
        roomRoot.appendChild(getRoomItem("good"+i));
        //msgRoot.appendChild(getMsgItem("omgUser","imomg"));
    }
}

populate();

function BindEvent(){
    document.getElementById("send-button").addEventListener("click",Send);
    
}

BindEvent();
const roomsList = [];
const rooms = {};
var selectedRoomEle;
var activeRoom;

const {LoginRequest,RegisterRequest,ChatMessageRequest,RoomControl,RoomRequest } = require('./request.js');
//const req = require('./main.js');

//操作
function Login(){

}

function Register(){

}

function Send(){
    console.log("its sent. or not.");
    text.value = "";
}

function SwitchRoom(e){
    console.log(e)
    if(e.target == selectedRoomEle){return;}
    e.target.classList.add(classRoomSelected);
    let ele = document.querySelector('#chat-pan');
    if(selectedRoomEle){
        selectedRoomEle.classList.remove(classRoomSelected);
        localStorage.setItem(selectedRoomEle.textContent.trim(),ele.querySelector(".vert-container").scrollTop);
    }
    selectedRoomEle = e.target;
    let name = selectedRoomEle.textContent.trim();
    
    if(rooms[name]){
        console.log("selected cache")
    }else{
        console.log("selected new")
        rooms[name] = htmlToElement(container);
    }
    ele.replaceChild(rooms[name],ele.firstChild);
    ele.firstChild.scrollTop =localStorage[name];
    activeRoom = rooms[name];
}

function Reconnect(){

}

//中间层
function GetMessage(username,text,room,isNotice){
    if(rooms[room]){
    }else{
        rooms[room] = htmlToElement(container);
    }
    let ele;
    if(isNotice){
        ele = getNoticeItem(text);
    }else{
        ele = getMsgItem(username,text);
    }
    rooms[room].appendChild(ele);
    if(activeRoom != rooms[room]){

    }
}

for(let i =0 ;i< 30;i++){
GetMessage("dagong","boooooyah0!","good0");
GetMessage("dagong","boooooyah1!","good1");
GetMessage("dagong","boooooyah2!","good2");
GetMessage("dagong","boooooyah3!","good3");
}
function PrepareRoomPage(){

}

function AppendNewMessageIntoRoom(){

}

function SaveToLocal(){

}