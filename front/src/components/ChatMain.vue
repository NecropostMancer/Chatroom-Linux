<template>
    <div id="root" >
        <div class="side-bar"></div>
        <div class="main" id="main">
            <div class="room">
                <div class="title-bar">
                    <div class="header-primary font-bolder room-title">房间列表</div>
                    <button aria-label="创建频道" tabindex="-1" type="button" class="icon-button" data-bs-toggle="modal" data-bs-target="#roomCreate">
                        <div class="contents-18-Yxp">
                            <svg class="addButtonIcon-2CbG1X" aria-hidden="false" width="18" height="18" viewBox="0 0 18 18">
                                <polygon fill-rule="nonzero" fill="currentColor" points="15 10 10 10 10 15 8 15 8 10 3 10 3 8 8 8 8 3 10 3 10 8 15 8">
                                </polygon>
                            </svg>
                        </div>
                    </button>
                    <div class="modal fade" id="roomCreate" data-bs-backdrop="static" data-bs-keyboard="false" tabindex="-1" aria-labelledby="roomCreateLabel" aria-hidden="true">
                        <div class="modal-dialog modal-dialog-centered">
                            <div class="modal-content">
                                <div class="modal-header">
                                    <h5 class="modal-title" id="roomCreateLabel">新房间</h5>
                                    <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
                                </div>
                                <div class="modal-body">
                                    <h6>
                                    创建房间...
                                    </h6>
                                    <div class="input-wrap">
                                        <input class="body" id="text" v-model="roomName" placeholder="输入房间名">
                                        <div class="tail">
                                            <button type="button" class="btn btn-secondary" data-bs-dismiss="modal" @click="CreateRoom(roomName)">创建</button>
                                        </div>
                                    </div>
                                    <h6>
                                    或者加入房间...
                                    </h6>
                                    <div class="input-wrap">
                                        <input class="body" id="text" v-model="roomID" @keyup.enter="Send" placeholder="输入房间ID">
                                        <div class="tail">
                                            <button type="button" class="btn btn-secondary" data-bs-dismiss="modal" @click="JoinRoom(roomID)">加入</button>
                                        </div>
                                    </div>
                                </div>
                                <div class="modal-footer">
                                    <button type="button" class="btn btn-secondary" data-bs-dismiss="modal">取消</button>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>

                <div class="room-content">
                    <div class="vert-container">
                        <div class = "room-item" v-for="room in rooms" :key = "room.id" @click="SwitchRoom(room,$event)" v-bind:class="{selected : room.isActive}">
                            {{room.roomname}}
                        </div>
                    </div>
                </div>
                
                <div class="footer">
                    <div class="nametag">
                        <div>{{$userState.loginUserName}}</div>
                        <div>{{$userState.loginShowName}}</div>
                    </div>
                    <button aria-label="用户设置" type="button" class="icon-button" data-bs-toggle="modal" data-bs-target="#settingdrop">
                        <div class="">
                            <svg aria-hidden="false" width="20" height="20" viewBox="0 0 24 24">
                                <path fill="currentColor" fill-rule="evenodd" clip-rule="evenodd" d="M19.738 10H22V14H19.739C19.498 14.931 19.1 15.798 18.565 16.564L20 18L18 20L16.565 18.564C15.797 19.099 14.932 19.498 14 19.738V22H10V19.738C9.069 19.498 8.203 19.099 7.436 18.564L6 20L4 18L5.436 16.564C4.901 15.799 4.502 14.932 4.262 14H2V10H4.262C4.502 9.068 4.9 8.202 5.436 7.436L4 6L6 4L7.436 5.436C8.202 4.9 9.068 4.502 10 4.262V2H14V4.261C14.932 4.502 15.797 4.9 16.565 5.435L18 3.999L20 5.999L18.564 7.436C19.099 8.202 19.498 9.069 19.738 10ZM12 16C14.2091 16 16 14.2091 16 12C16 9.79086 14.2091 8 12 8C9.79086 8 8 9.79086 8 12C8 14.2091 9.79086 16 12 16Z"></path>
                            </svg>
                        </div>
                    </button>
                    <div class="modal fade" id="settingdrop" data-bs-backdrop="static" data-bs-keyboard="false" tabindex="-1" aria-labelledby="settingdropLabel" aria-hidden="true">
                        <div class="modal-dialog modal-dialog-centered">
                            <div class="modal-content">
                                <div class="modal-header">
                                    <h5 class="modal-title" id="settingdropLabel">用户设置</h5>
                                    <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
                                </div>
                                <div class="modal-body">
                                    <h6>
                                    新用户名
                                    </h6>
                                    <div class="input-wrap">
                                        <input class="body" id="text" v-model="newName" placeholder="新用户名">
                                        <div class="tail">
                                        </div>
                                    </div>
                                </div>
                                <div class="modal-footer">
                                    <button type="button" class="btn btn-primary" data-bs-dismiss="modal" @click="ChangeName">保存</button>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
            <div class="context">
                <div class="title-bar">
                    <div class="header-primary font-normal">{{rooms[activeRoom] ?rooms[activeRoom].roomname : ""}}</div>
                    <div class="divider"></div>
                    <div class="desc font-light">#{{rooms[activeRoom] ?rooms[activeRoom].roomid : ""}}</div>
                    <div class="tools">
                        <div class="tool-wrap">
                        <svg x="0" y="0" class="icon icon-notice" aria-hidden="false" width="24" height="24" viewBox="0 0 24 24" fill="none" v-bind:class="{active : rooms[activeRoom] ?rooms[activeRoom].noDistrub : false}" @click="rooms[activeRoom].noDistrub = !rooms[activeRoom].noDistrub">
                            <path fill="currentColor" fill-rule="evenodd" clip-rule="evenodd" d="M18 9V14C18 15.657 19.344 17 21 17V18H3V17C4.656 17 6 15.657 6 14V9C6 5.686 8.686 3 12 3C15.314 3 18 5.686 18 9ZM11.9999 21C10.5239 21 9.24793 20.19 8.55493 19H15.4449C14.7519 20.19 13.4759 21 11.9999 21Z"></path>
                        </svg>
                        </div>
                        <div class="tool-wrap">
                        <svg x="0" y="0" class="icon" width="24" height="24" viewbox="0 0 24 24" v-bind:class="{active : showMember}" @click="showMember = !showMember">
                            <path fill="currentColor" fill-rule="evenodd" clip-rule="evenodd" d="M14 8.00598C14 10.211 12.206 12.006 10 12.006C7.795 12.006 6 10.211 6 8.00598C6 5.80098 7.794 4.00598 10 4.00598C12.206 4.00598 14 5.80098 14 8.00598ZM2 19.006C2 15.473 5.29 13.006 10 13.006C14.711 13.006 18 15.473 18 19.006V20.006H2V19.006Z"></path>
                        </svg>
                        </div>
                        <div class="tool-wrap" data-bs-toggle="modal" data-bs-target="#roomsettingdrop" v-bind:class="{hide:activeRoom == 0}">
                        <svg aria-hidden="false" width="20" height="20" viewBox="0 0 24 24">
                            <path fill="currentColor" fill-rule="evenodd" clip-rule="evenodd" d="M19.738 10H22V14H19.739C19.498 14.931 19.1 15.798 18.565 16.564L20 18L18 20L16.565 18.564C15.797 19.099 14.932 19.498 14 19.738V22H10V19.738C9.069 19.498 8.203 19.099 7.436 18.564L6 20L4 18L5.436 16.564C4.901 15.799 4.502 14.932 4.262 14H2V10H4.262C4.502 9.068 4.9 8.202 5.436 7.436L4 6L6 4L7.436 5.436C8.202 4.9 9.068 4.502 10 4.262V2H14V4.261C14.932 4.502 15.797 4.9 16.565 5.435L18 3.999L20 5.999L18.564 7.436C19.099 8.202 19.498 9.069 19.738 10ZM12 16C14.2091 16 16 14.2091 16 12C16 9.79086 14.2091 8 12 8C9.79086 8 8 9.79086 8 12C8 14.2091 9.79086 16 12 16Z"></path>
                        </svg>
                        </div>
                        <div class="tool-wrap"  v-bind:class="{hide:!IsOwner()}">
                        <svg t="1617267930811" class="icon" viewBox="0 0 1024 1024" version="1.1" xmlns="http://www.w3.org/2000/svg" p-id="2776" width="24" height="24" v-bind:class="{active : rooms[activeRoom] ?rooms[activeRoom].locked : false}" @click="LockRoom">
                            <path fill="currentColor" fill-rule="evenodd" clip-rule="evenodd" d="M337.92 456.704v-81.92c0-112.64 92.16-204.8 204.8-204.8s204.8 92.16 204.8 204.8v81.92h40.96c45.056 0 81.92 36.864 81.92 81.92v245.76c0 45.056-36.864 81.92-81.92 81.92h-491.52c-45.056 0-81.92-36.864-81.92-81.92v-245.76c0-45.056 36.864-81.92 81.92-81.92h40.96z m204.8-204.8c-67.584 0-122.88 55.296-122.88 122.88v81.92h245.76v-81.92c0-67.584-55.296-122.88-122.88-122.88z" p-id="2777"></path>
                        </svg>
                        </div>
                        <div class="modal fade" id="roomsettingdrop" data-bs-backdrop="static" data-bs-keyboard="false" tabindex="-1" aria-labelledby="roomsettingdropLabel" aria-hidden="true">
                            <div class="modal-dialog modal-dialog-centered">
                                <div class="modal-content">
                                    <div class="modal-header">
                                        <h5 class="modal-title" id="roomsettingdropLabel">房间设置</h5>
                                        <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
                                    </div>
                                    <div class="modal-body">
                                        <div v-bind:class="{hide:activeRoom == 0}" style="font-weight: 600;line-height: 18px;">
                                        {{rooms[activeRoom] ? rooms[activeRoom].roomname : ""}}
                                        </div>
                                        <div v-bind:class="{hide:activeRoom == 0}" style="font-weight: 600;line-height: 18px;margin-bottom:8px">
                                        ID#{{rooms[activeRoom] ? rooms[activeRoom].roomid : ""}}
                                        </div>
                                        <hr>
                                        <h6 v-bind:class="{hide:!IsOwner()}">
                                        新房间名
                                        </h6>
                                        <div class="input-wrap" v-bind:class="{hide:!IsOwner()}">
                                            <input class="body" id="text" v-model="newName" placeholder="新房间名">
                                            <div class="tail">
                                                <button type="button" class="btn btn-primary" data-bs-dismiss="modal">保存</button>
                                            </div>
                                        </div>
                                        <button type="button" class="btn btn-warning" v-bind:class="{hide:activeRoom == 0}" style="margin-right:24px" data-bs-toggle="modal" data-bs-target="#confirm" @click="dangerousOp = 0" data-bs-dismiss="modal">退出房间</button>
                                        <div v-bind:class="{hide:activeRoom != 0}">
                                            这是默认的公共频道，您无法对其进行操作。
                                        </div>
                                        <button type="button" class="btn btn-danger" v-bind:class="{hide:!IsOwner()}"  data-bs-toggle="modal" data-bs-target="#confirm" @click="dangerousOp = 1" data-bs-dismiss="modal">删除房间</button>
                                    </div>
                                    <div class="modal-footer">
                                        
                                    </div>
                                </div>
                            </div>
                        </div>
                        <div class="modal fade" id="confirm" data-bs-backdrop="static" data-bs-keyboard="false" tabindex="-2" aria-labelledby="confirmLabel" aria-hidden="true">
                            <div class="modal-dialog modal-dialog-centered">
                                <div class="modal-content">
                                    <div class="modal-header">
                                        <h5 class="modal-title" id="confirmLabel">确认操作</h5>
                                    </div>
                                    <div class="modal-body">
                                        <div>{{notice[dangerousOp]}}</div>
                                    </div>
                                    <div class="modal-footer">
                                        <button type="button" class="btn btn-secondary" data-bs-dismiss="modal">我点错了</button>
                                        <button type="button" class="btn btn-primary" data-bs-dismiss="modal" @click="doQuitorDelete">我确定</button>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
                <div class="chat">
                    <div class="text-area">
                        <div class="text">
                            <div v-for="room in rooms" v-show="activeRoom == room.roomid" :key="room.roomid" class = "vert-container">
                                <div v-for="msg in room.msg" :key = "msg.id" class="">
                                    <div v-if="true || msg.type == 1" class = "msg-wrap">
                                        <div class = "msg-header">{{msg.showname}}:</div>
                                        <div class = "msg-body">{{msg.msg}}</div>
                                    </div>
                                    <div v-else class = "notice-item">
                                        <span class="font-notice">{{msg.msg}}</span>
                                    </div>
                                </div>
                            </div>
                        </div>
                        <div class="input">
                            <form @submit.prevent>
                                <div class="input-wrap">
                                    <div class="head"></div>
                                    <input class="body" id="text" v-model="message" @keyup.enter="Send" placeholder="回车发送消息">    
                                    <div class="tail">
                                        <svg x="0" y="0" class="icon" width="24" height="24" viewbox="0 0 24 24">
                                            <path fill="currentColor" fill-rule="evenodd" clip-rule="evenodd" d="M481.508,210.336L68.414,38.926c-17.403-7.222-37.064-4.045-51.309,8.287C2.86,59.547-3.098,78.551,1.558,96.808
                                                L38.327,241h180.026c8.284,0,15.001,6.716,15.001,15.001c0,8.284-6.716,15.001-15.001,15.001H38.327L1.558,415.193
                                                c-4.656,18.258,1.301,37.262,15.547,49.595c14.274,12.357,33.937,15.495,51.31,8.287l413.094-171.409
                                                C500.317,293.862,512,276.364,512,256.001C512,235.638,500.317,218.139,481.508,210.336z"></path>
                                        </svg>
                                    </div>
                                </div>
                            </form>
                            
                        </div>
                    </div>
                    <div class="member" v-show="showMember">
                        <h2>在线 - 1</h2>
                        <div>1</div>
                    </div>
                </div>
            </div>            
        </div>       
    </div>
</template>

<script>
import EditableText from "./EditableDiv.vue";
export default {
    name: 'ChatMain',
    components:{
        EditableText
    },
    data () {
        return {
            rooms:{},
            selectedRoomEle:'',
            activeRoom:0,
            message:'',
            showMember:true,
            roomName:"",
            roomID:"",
            newName:"",
            dangerousOp:0,
            notice:["你确认要退出该房间吗？你将不再会收到任何来自该房间的信息！","你确认要删除该房间吗？该房间的所有消息记录都将消失！"]
        }
    },
    props: {
        request: Object
    },
    mounted : function(){
        //var text = document.getElementById("text");
        //this.autoTextarea(text);
        this.QueryAllRoom(true); 
        this.request.OnChatMessageRecieve(this.GetMessage);
        setTimeout(()=>{document.getElementById("main").classList.add("show")},0);
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
        IsOwner : function(){
            return this.$data.rooms[this.activeRoom] ? this.$data.rooms[this.activeRoom].creator == this.$userState.loginUserName : false;
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
        QueryAllRoom : function (init)  {

            this.request.RoomRequest(3,0,"null").then((res)=>{
                console.log(res);
                this.AppendRoom(res);
                if(init && init == true){
                    this.$data.rooms[0].isActive = true;
                    this.$data.activeRoom = 0;
                }
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
            room.unRead = 0;
            room.isActive = true;
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
                if(msg.roomid!=this.activeRoom){
                    rooms.unRead++;
                }
            }
        },
        AppendMessage : function(msg){
            this.$data.rooms[msg.roomid].msg.push(msg);
        },
        AppendRoom : function(list){
            for(let i = 0;i<list.length;i++){
                if(this.$data.rooms[list[i].roomid ? list[i].roomid : 0]){
                    continue;
                }
                this.$data.rooms[list[i].roomid ? list[i].roomid : 0] = {
                    roomid : list[i].roomid ? list[i].roomid : 0,
                    roomname : list[i].roomname,
                    creator : list[i].creator,
                    msg : [],
                    isActive : false,
                    noDistrub : false,
                    locked: false,
                    unRead: 0
                }
            }
        },
        JoinRoom : function(roomid){
            this.request.RoomRequest(1,roomid.trim(),'').then((res)=>{
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
            this.request.RoomRequest(0,0,name.trim()).then((res)=>{
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
                if(res.error.code != 4 || typeof res.error.code == 'undefined'){
                    this.$data.rooms = Object.keys(this.$data.rooms).filter(key =>
                        parseInt(key) !== roomid).reduce((obj, key) =>
                        {
                            obj[key] = this.$data.rooms[key];
                            return obj;
                        }, {}
                    );
                    this.activeRoom = 0;
                    this.SwitchRoom(this.$data.rooms[0]);
                }else{
                    throw new Error("roomNotleave");
                }
                console.log(res);
            }).catch((e)=>{
                console.log(e);
            })
        },
        LockRoom : function(){
            this.request.RoomControl(this.rooms[this.activeRoom].locked?1:0,this.activeRoom,'').then((res)=>{
                if(res.error.code != 4){
                    this.$data.rooms[this.activeRoom].locked = !this.$data.rooms[this.activeRoom].locked;
                }else{
                    throw new Error("roomNotlocked");
                }
                console.log(res);
            }).catch((e)=>{
                console.log(e);
            })
        },
        ChangeName : function(){
            this.request.ChangeNameRequest(this.newName);
            this.$$userState.loginShowName = this.newName
        },
        doQuitorDelete : function(){
            if(this.$data.dangerousOp == 0){
                this.LeaveRoom(this.$data.activeRoom);
            }else{
                this.LeaveRoom(this.$data.activeRoom);
            }
        }
    }
}
</script>

<style scoped lang="scss">

h1,h2,h3{
    color:var(--header-primary)
}
h2{
    font-weight: 600;
    font-size: 16px;
}
h3{
    font-weight: 500;
    font-size: 16px;
}
h4,h5{
    color:var(--header-secondary)
}
h6{
    color:var(--text-normal);
    margin-bottom: 8px;
    font-size: 12px;
    line-height: 16px;
    font-weight: 600;
    text-align: left;
}
.modal-title{
    text-align: center;
    font-weight: 800;
    color:var(--header-primary);
}
input{
    background-color: var(--deprecated-text-input-bg);
    border: 1px solid var(--border-light-dark);
    color: var(--text-normal);
}
.header-primary{
    color:var(--header-primary)
}
.font-bolder{
    font-weight: 600;
}
.font-normal{
    font-weight: 500;
}
.font-light{
    font-weight: 400;
}
div{
    color:var(--text-normal)
}
svg.icon{
    display: block;
    width: 24px;
    height: 24px;
    color: var(--interactive-normal);
}
svg.icon:hover{
    color: var(--interactive-hover);
}
svg.icon.active{
    color: var(--interactive-active);
}
svg.icon-notice.active{
    color: rgb(230, 77, 77);
}
#root {
    display: flex;
    margin: auto;
    height: 870px;
    width: 100%;
    background-color: var(--background-tertiary);
}
.side-bar{
    width: 72px;
    padding: 12px 0 0;
    flex: 0 1 auto;
    background-color: var(--background-tertiary);
}
.main{
    display: flex;
    background-color: var(--background-primary);
    width: 100%;
    opacity: 0;
    transition: opacity 1s ease-in-out;
}
.main.show{
    opacity: 1;
}
.room{
    width:240px;
    flex: 0 0 auto;
    background-color: var(--background-secondary);
    display: flex;
    flex-direction: column;
}
.room-title{
    flex: 1 1 auto;
}
.room-content{
    flex: 1 1 auto;
    position: relative;
}
.context{
    display: flex;
    flex-direction: column;
    background-color: var(--background-primary);
    flex: 1 0 auto;
}
.title-bar{
    height: 48px;
    width: 100%;
    flex: 0 0 auto;
    box-shadow: 0 1px 0 rgba(4,4,5,0.2),0 1.5px 0 rgba(6,6,7,0.05),0 2px 0 rgba(4,4,5,0.05);
    display: flex;
    align-items: center;
    line-height: 20px;
    font-size: 16px;
    padding: 0 16px;
}
.footer{
    background-color: var(--background-secondary-alt);
    height: 48px;
    flex: 0 0 auto;
    padding: 6px;
    display: flex;
    align-items: center;
    padding: 0 16px;
}
.footer .nametag{
    flex: 1 0 auto;
    
}
.icon-button{
    width:32px;
    height: 32px;
    display: flex;
    align-items: center;
    justify-content: center;
    border: none;
    border-radius: 4px;
    line-height: 0;
    background-color: rgba(0, 0, 0, 0);
}
.divider{
    width: 1px;
    height: 24px;
    margin: 0 16px;
    background-color: var(--background-accent);
    flex: 0 0 auto;
}
.tools{
    display: flex;
    align-items: center;
}
.tools .tool-wrap{
    padding: 0 8px;
    vertical-align: center;
}
.desc{
    flex: 1 1 auto;
    color: var(--header-secondary);
    font-size: 12px;
}
.chat{
    display: flex;
    flex: 1 1 auto;
}
.text-area{
    display: flex;
    flex-direction: column;
    flex: 1 1 auto;
}
.text{
    flex: 1 1 auto;
    display: flex;
    flex-direction: column;
    position: relative;
}
.text .msg-wrap{
    padding: 0px 16px;
    width: 100%;
    display: flex;
    color: var(--text-normal)
}
.text .msg-wrap .msg-header{
    margin-right: 8px;
    color: var(--header-primary);
}
.input{
    
}
.input form{
    padding: 0 16px;
}
.input-wrap{
    height: 44px;
    border-radius: 4px;
    background-color: var(--background-primary);
    margin-bottom: 24px;
    display: flex;
}
.input-wrap .head{
    height: 100%;
    width: 56px;
    flex: 0 0 auto;
    background-color: var(--background-accent);
}
.input-wrap .body{
    flex: 1 1 auto;
    background-color: var(--background-accent);
    padding: 11px;
    overflow: hidden;
    border: none;
    color: var(--text-normal)
}
.input-wrap .tail{
    flex: 0 0 auto;
    background-color: var(--background-accent); 
}
.input-wrap .tail button{
    height:100%;
}

.member{
    width: 240px;
    flex: 0 0 auto;
    background-color: var(--background-secondary);
    position: relative;
    padding: 10px 16px;
}
.room-item{
    padding: 6px 10px;
    margin: 1px 6px;
    color: var(--text-normal);
    border-radius: 2px;
}

.room-item:hover{
    background-color: var(--background-secondary-alt);
    color: var(--header-secondary)
}

.room-item.selected{
    background-color: var(--background-primary);
    color: var(--header-primary)
}

.modal-content{
    border: none;
    border-radius: 4px;
    background-color: var(--background-primary);
}

.modal-header{
    background-color: var(--background-accent);
    color: var(--header-primary);
    border: none;
    border-radius: 4px;
}

.modal-body{
    background-color: var(--background-primary);
    color: var(--header-secondary);
}

.btn-close{
    color: var(--interactive-normal);
}

.btn-close:hover{
    color: var(--interactive-hover);
}

.modal-footer{
    background-color: var(--background-primary);
    color: var(--header-secondary);
    border: none;
    border-radius: 4px;
}

.vert-container {
    display: flex;
    flex-direction: column;

    width: 100%;
    flex: 1 1 auto;
    overflow-x: hidden;
    overflow-y: overlay;
    position: absolute;
    top: 0;
    bottom: 0;
    left: 0;
    right: 0;
    user-select: none;
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
    background-color: var(--scrollbar-auto-track);
    z-index: 999;
    border-radius: 4px;
}

.vert-container::-webkit-scrollbar-thumb {
    background-color: var(--scrollbar-thin-thumb);
    border-radius: 4px;
}

.vert-container::-webkit-scrollbar-button {
    background-color: var(--scrollbar-thin-thumb);
    display: none;
}

.vert-container::-webkit-scrollbar-corner {
    /*background-color: black;*/
    border-radius: 4px;
}

.nametag {
    display: flex;
    flex-direction: column;
    align-items: center;
    overflow: hidden;
}
.nametag div:first-child{
    font-weight: 600;
    line-height: 18px;
    color: var(--header-primary);
}

.nametag div:last-child{
    font-size: 12px;
    line-height: 13px;
    color: var(--header-secondary);
}
.hide{
    display: none;
}
</style>