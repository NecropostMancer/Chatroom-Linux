<template>
  <div id="root">
        <div id = "room-pan">
            
            <div class = "vert-container">
                <div class = "room-item" v-for="room in rooms" :key = "room.id" @click="SwitchRoom(room,$event)" v-bind:class="{selected : room.isActive}">
                    <span class = "font-bolder" >{{room.roomname}}</span>
                </div>
            </div>
            
            <div id = "setting" class="bottom-stick">
                123
            </div>
        </div>
        <div id = "chat-pan">
            
            <div v-for="room in rooms" v-show="activeRoom == room.roomid" :key="room.roomid" class = "vert-container">
                <div v-for="msg in room.msg" :key = "msg.id" class="item-wrapper">
                    <div v-if="true || msg.type == 1" class = "msg-item">
                        <span class = "font-bolder username">{{msg.showname}}:</span>
                        <span class = "font-normal message">{{msg.msg}}</span>
                    </div>
                    <div v-else class = "notice-item">
                        <span class="font-notice">{{msg.msg}}</span>
                    </div>
                </div>
            </div>
            
            <div id = "input" class="bottom-stick">
                <textarea id="text" rows = "1" v-model="message"></textarea>
                <span id = "send-button">
                    <a @click="Send">发送</a>
                </span>
            </div>
        </div>
    </div>
</template>

<script>
export default {
    name: 'ChatMain',
    data () {
        return {
            rooms:{},
            selectedRoomEle:'',
            activeRoom:0,
            message:''
        }
    },
    props: {
        request: Object
    },
    mounted : function(){
        var text = document.getElementById("text");
        this.autoTextarea(text);
        this.QueryAllRoom();
        this.request.OnChatMessageRecieve(this.GetMessage);
    },
    methods : {
        autoTextarea : function (elem, extra, maxHeight) {
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
        },
        Send : function (){
            
            if(this.$data.message[0] == '/'){
                this.SendCommand(this.$data.message.slice(1));
            }else{
                this.request.ChatMessageRequest(this.message,this.activeRoom).then((res)=>{
                    console.log(res);
                }).catch((e)=>{
                    console.log(e);
                })
            }
            this.$data.message = '';
        },
        SendCommand(string){
            if(string[0] == 'c'){
                this.CreateRoom(string.slice(2).trim())
            }else if(string[0] == 'j'){
                this.JoinRoom(parseInt(string.slice(2).trim()));
            }else if(string[0] == 'l'){
                this.LeaveRoom(this.$data.activeRoom);
            }else{
                console.log("not found command.");
            }
        },
        GetMessage : function () {
        }, 
        QueryAllRoom : function ()  {
            this.request.RoomRequest(3,0,"null").then((res)=>{
                console.log(res);
                this.AppendRoom(res);
            }).catch((e)=>{
                console.log(e);
            })
        },
        
        SwitchRoom : function(room,event){
            console.log(room)
            if(this.activeRoom == room.roomid){
                room.isActive = true;
                return;
            }
            if(this.rooms[this.activeRoom]){
                this.rooms[this.activeRoom].isActive = false;
            }
            this.activeRoom = room.roomid;
            room.isActive = true;
            //event.target.classList.remove(classRoomSelected);
            //localStorage.setItem(room.roomname.trim(),event.target.querySelector(".vert-container").scrollTop);
            /*
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
            */
        },
        GetMessage : function(msg){
            console.log("ok");
            if(msg.roomid){
                this.AppendMessage(msg)
            }else{
                this.AppendMessage({
                    msg : msg.msg,
                    roomorder : msg.roomorder,
                    username : msg.username,
                    showname : msg.showname,
                    type : msg.type,
                    roomid: 0
                })
            }
        },
        AppendMessage : function(msg){
            this.$data.rooms[msg.roomid].msg.push(msg);
        },
        AppendRoom : function(list){
            for(let i = 0;i<list.length;i++){
                this.$data.rooms[list[i].roomid ? list[i].roomid : 0] = {
                    roomid : list[i].roomid ? list[i].roomid : 0,
                    roomname : list[i].roomname,
                    msg : [],
                    isActive : false
                }
            }
        },
        JoinRoom : function(roomid){
            this.request.RoomRequest(1,roomid,'').then((res)=>{
                if(res.error.code != 4){
                    this.QueryAllRoom();
                }else{
                    throw new Error("roomNotjoin");
                }
                console.log(res);
            }).catch((e)=>{
                console.log(e);
            })
        },
        CreateRoom : function(name){
            this.request.RoomRequest(0,0,name).then((res)=>{
                console.log(res);
                if(res.error.code != 2){
                    this.QueryAllRoom();
                }else{
                    throw new Error("roomNotcreate");
                }
                console.log(res);
            }).catch((e)=>{
                console.log(e);
            })
        },
        LeaveRoom : function(roomid){
            this.request.RoomRequest(2,roomid,'').then((res)=>{
                if(res.error.code != 4){
                    this.$data[roomid] = undefined;
                }else{
                    throw new Error("roomNotleave");
                }
                console.log(res);
            }).catch((e)=>{
                console.log(e);
            })
        }
    }
}
</script>

<style scoped lang="scss">
#root {
    display: flex;
    margin: auto;
    height: 100vh;

}
#room-pan {
    position: relative;
    width: 30%;
    height: 100%;
    background-color: rgb(0, 5, 48);
    user-select: none;
}

#chat-pan {
    position: relative;
    margin-left: 20px;
    margin-right: 10px;
    width: 70%;
    height: 100%;
    background-color: rgb(255, 255, 255);
    user-select: none;
}

#input {
    position: absolute;
    bottom: 0;
    width: 100%;
    background-color: white;
    margin-left: -5px;
    margin-left: -10px;
    line-height: 32px;
}

#setting {
    position: absolute;
    bottom: 0;
    width: 100%;
    height: 40px;
    background-color: rgb(0, 5, 48);
    color: white;
}

#input textarea {
    width: 90%;
    font-size: 1.2em;
    padding: 5px;
}

.vert-container {
    display: flex;
    flex-direction: column;

    width: 100%;
    max-height: 100%;
    overflow-x: hidden;
    overflow-y: overlay;

    user-select: none;
}

.bottom-stick {
    position: absolute;
    bottom: 0;
    width: 100%;
}

.room-item{
    box-sizing: border-box;
    padding-left: 6px;
    height: 32px; 
    line-height: 32px;
    width: 100%;
    background-color: rgb(70, 81, 110);
    margin-bottom: 5px;
    flex-shrink: 0;
    
    
    color:rgb(144, 150, 168);
}

.room-item:hover{
    color: white;
}

.room-item span{
    user-select: none;
    pointer-events: none;
}

.selected{
    background-color: rgb(37, 79, 177);
    color: white;
}

.room-item:last-child{
    margin-bottom: 200px;
}

.item-wrapper:last-child{
    margin-bottom: 150px;
    flex-shrink: 0;
}

.msg-item{
    
}

.msg-item:last-child{
    
}

.notice{
    position: relative;
}

.notice::after{
    box-sizing: border-box;
    content:'';
    position: absolute;
    background-color: crimson;
    border: 1px crimson solid;
    width: 4px;
    height: 32px;
    right:10px;
}

.font-bolder {
    font-size: normal;
    font-weight: 1000;
}

.font-normal {

}

.notice-item {
    position: relative;
    margin-top: 10px;
    margin-bottom: 10px;
    text-align: center;
    overflow: hidden;
    min-height: 32px;
}

.notice-item::before,
.notice-item::after {
    background-color: darkgray;
    content: "";
    display: inline-block;
    height: 1px;
    position: relative;
    vertical-align: middle;
    width: 50%;
}

.notice-item::before {
    right: 0.5em;
    margin-left: -50%;
}
.notice-item::after {
    left: 0.5em;
    margin-right: -50%;
}

.notice-item:last-child {

}

.font-notice {
    font-size: small;
    color:darkgray;

}

.vert-container::-webkit-scrollbar{
    width: 8px;
    height: 8px;
}

.vert-container::-webkit-scrollbar-track {
    background-color: #ddd;
    z-index: 999;
}

.vert-container::-webkit-scrollbar-thumb {
    background-color: rgba(0, 0, 0, 0.6);
    border-radius: 4px;
}

.vert-container::-webkit-scrollbar-button {
    background-color: #888;
    display: none;
}

.vert-container::-webkit-scrollbar-corner {
    /*background-color: black;*/
}
</style>