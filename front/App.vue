<template>
    <div>
        <ChatMain v-if="log" :request = "ChatPageMethod"></ChatMain>
        <Title v-else :request = "LoginPageMethod"></Title>
    </div>
</template>

<script>

import ChatMain from './src/components/ChatMain.vue'
import Title from './src/components/Title.vue'
import {timeoutPromise} from './src/utils.js'
import protobuf from 'protobufjs';

import net from 'net';

export default {
    name: 'App',
    data() {
        return {
            protoStorage : {
                "ChatMessageRequest":null,
                "ChatMessageSend":null,
                "ChatResponse":null,
                "Error":null,
                "RoomRequest":null,
                "RoomResponse":null,
                "RoomControl":null,
                "LoginRequest":null,
                "LoginResponse":null,
                "RegisterRequest":null,
                "ChangeNameRequest":null,
                "RegularResponse":null,
                "Req":null,
                "Res":null
            },
            
            prepared : false,
            ProtobufReciever : {
                _onRecieveMessageResponse:[],
                _onRecieveRoomResponse:[],
                _onRecieveAuthResponse:undefined,
                _onChatMessageRecieve:function(e){console.log(e)},
                onRecieveMessageResponse:function(cb){
                    this._onRecieveMessageResponse.push(cb);
                },
                onRecieveRoomResponse:function(cb){
                    this._onRecieveRoomResponse.push(cb);
                },
                removeMessageResponse:function(cb){
                    let ind = this._onRecieveMessageResponse.indexOf(cb);
                    this._onRecieveMessageResponse.splice(ind,1);
                },
                removeRoomResponse:function(cb){
                    let ind = this._onRecieveRoomResponse.indexOf(cb);
                    this._onRecieveRoomResponse.splice(ind,1);
                },
                onAuth(cb){
                    this._onRecieveAuthResponse = cb;
                }
            },
            LoginPageMethod : {
                LoginRequest : this.LoginRequest,
                RegisterRequest : this.RegisterRequest,
                SwitchPage : this.SwitchPage
            },
            ChatPageMethod : {
                ChatMessageRequest : this.ChatMessageRequest,
                RoomRequest: this.RoomRequest,
                RoomControl: this.RoomControl,
                SwitchPage : this.SwitchPage,
                OnChatMessageRecieve : this.OnChatMessageRecieve
            },
            log :false,
            that: this
        }
    },
    mounted : function() {
        this.$socket.on("data",this.ProtobufRead);
        this.LoadAllProto();
    },
    methods : {
        SwitchPage : function(){
            this.$data.log = !this.$data.log;
        },
        ProtobufRead : function(payload){
            var res = this.protoStorage["Res"].decode(payload);
            console.log(res);
            if(res.loginResponse){
                this.ProtobufReciever._onRecieveAuthResponse(res.loginResponse);
                if(res.loginResponse.username){
                    this.$userState.username = res.loginResponse.username;
                    this.$userState.tokenFromServer = res.loginResponse.token;
                }
            }else if(res.chatMessageRequest){
                for(let i = 0 ;i<ProtobufReciever._onRecieveMessageResponse.length;i++){
                    this.ProtobufReciever._onRecieveMessageResponse[i]();
                }
            }else if(res.chatMessageSend){
                this.ProtobufReciever._onChatMessageRecieve(res.chatMessageSend);
            }else if(res.roomResponse){
                if(res.rooms.length!=0){
                    this.ProtobufReciever._onRecieveRoomResponse[0](res.rooms);
                }else{
                    this.ProtobufReciever._onRecieveRoomResponse[0](res.roomResponse);
                }
            }
            else{
            console.log("unsupported message type.");
            }
        },
        ProtobufSend : function (messageName,payload){ 
            this._ProtobufSend(messageName,payload);
        },
        _ProtobufSend: function (messageName,payload){
            let message = this.protoStorage[messageName].create(payload);
            console.log(message);
            let bytes = this.protoStorage[messageName].encode(message).finish();
            console.log(bytes);
            this.$socket.write(bytes);
        },
        LoadAllProto : async function (){
            let root = await protobuf.load("./Chat.proto");
            this.$data.protoStorage["ChatMessageRequest"] = root.lookupType("ChatMessageRequest");
            this.$data.protoStorage["ChatMessageSend"] = root.lookupType("ChatMessageSend");
            this.$data.protoStorage["ChatResponse"] = root.lookupType("ChatResponse");
            root = await protobuf.load("./Error.proto");
            this.$data.protoStorage["Error"] = root.lookupType("Error");
            root = await protobuf.load("./Room.proto");
            this.$data.protoStorage["RoomRequest"] = root.lookupType("RoomRequest");
            this.$data.protoStorage["RoomResponse"] = root.lookupType("RoomResponse");
            this.$data.protoStorage["RoomControl"] = root.lookupType("RoomControl");
            root = await protobuf.load("./User.proto");
            this.$data.protoStorage["LoginRequest"] = root.lookupType("LoginRequest");
            this.$data.protoStorage["LoginResponse"] = root.lookupType("LoginResponse");
            this.$data.protoStorage["RegisterRequest"] = root.lookupType("RegisterRequest");
            this.$data.protoStorage["ChangeNameRequest"] = root.lookupType("ChangeNameRequest");
            this.$data.protoStorage["RegularResponse"] = root.lookupType("RegularResponse");
            root = await protobuf.load("./Wrapper.proto");
            this.$data.protoStorage["Req"] = root.lookupType("WrapperClientMessage");
            this.$data.protoStorage["Res"] = root.lookupType("WrapperServerMessage");
        },
        LoginRequest : function (username,password){
            var that = this;
            return timeoutPromise(
                new Promise((resolve,reject) => {
                    this.ProtobufReciever.onAuth(function(response){
                        resolve(response);
                    });
                    try{
                        this.ProtobufSend("Req",{
                            test: 1,
                            loginRequest:{
                            username : username,
                            password : password
                            }
                        });
                    }catch(e){
                        reject(e);
                    }
            }),5000);
            
        },
        RegisterRequest : function (username,password,showname){
    
            return timeoutPromise(
                new Promise((resolve,reject) => {
                    this.ProtobufReciever.onAuth(function(response){
                        resolve(response); 
                    });
                    try{
                        this.ProtobufSend("Req",{
                            test: 1,
                            registerRequest:{
                            username : username,
                            password : password,
                            showname : showname
                            }
                        });
                    }catch(e){
                        reject(e);
                    }
            }),5000);
        },
        ChatMessageRequest : function (msg,roomid){
            var that = this;
            return timeoutPromise(
                new Promise((resolve,reject) => {
                    this.ProtobufReciever.onRecieveMessageResponse(function(response){
                        if(yes,itsWhatMessageImWaitingFor,true){//<--implmenting this one.
                            ProtobufReciever.removeMessageResponse(this);
                            resolve(response);
                        }
                    });
                    try{
                        this.ProtobufSend("Req",{
                            test:1,
                            chatMessageRequest:{
                            username : that.$userState.loginUserName,
                            token : that.$userState.tokenFromServer,
                            msg : msg,
                            roomid : roomid,
                            random : 'md5(msg)'
                            }
                        });
                    }catch(e){
                        reject(e);
                    }
                }),5000);
        },
        RoomRequest : function(opreation,roomid,payload){
            var op = undefined;
            switch (opreation){
                case 0:
                    op = this.$data.protoStorage["RoomRequest"].Operation.CREATE;
                    break;
                case 1:
                    op = this.$data.protoStorage["RoomRequest"].Operation.JOIN;
                    break;
                case 2:
                    op = this.$data.protoStorage["RoomRequest"].Operation.LEAVE;
                    break;
                case 3:
                    op = this.$data.protoStorage["RoomRequest"].Operation.QUERY;
                    break;
            }
            var that = this;
            return timeoutPromise(
                new Promise((resolve,reject) => {
                    
                    this.ProtobufReciever.onRecieveRoomResponse(function(response){
                        
                            that.ProtobufReciever.removeRoomResponse(this);
                            resolve(response);
                        
                    });
                    try{
                        var that = this;
                        this.ProtobufSend("Req",{
                            test:1,
                            roomRequest:{
                                roomid : roomid,
                                username : that.$userState.loginUserName,
                                operation : op,
                                payload : payload
                            }
                        });
                    }catch(e){
                        reject(e);
                    }
                }),5000);
        },
        RoomControl : function(opreation,roomid,extra){
            var op = undefined;
            var that = this;
            switch (opreation){
                case 0:
                    op = this.$data.protoStorage["RoomControl"].Operation.LOCK;
                    break;
                case 1:
                    op = this.$data.protoStorage["RoomControl"].Operation.UNLOCK;
                    break;
                case 2:
                    op = this.$data.protoStorage["RoomControl"].Operation.KICK;
                    break;
                case 3:
                    op = this.$data.protoStorage["RoomControl"].Operation.PROMOTE;
                    break;
            }
           return timeoutPromise(
                new Promise((resolve,reject) => {
                    this.ProtobufReciever.onRecieveRoomResponse(function(response){
                        
                            that.ProtobufReciever.removeRoomResponse(this);
                            resolve(response);
                        
                    });
                    try{
                        var that = this;
                        this.ProtobufSend("Req",{
                            test:1,
                            roomControl:{
                                roomid : roomid,
                                username : that.$userState.loginUserName,
                                operation : op,
                                payload : payload
                            }
                        });
                    }catch(e){
                        reject(e);
                    }
                }),5000);
        },
        OnChatMessageRecieve : function(cb){
            this.ProtobufReciever._onChatMessageRecieve = cb;
        }
    },
    components: {
        ChatMain,
        Title
    }
}
</script>

<style lang="scss">
    html, body {
        width: 100%;
        height: 100%;
        margin: 0;
        padding: 0;
        user-select: none;
    }
    webview {
        display: block;   /* iframes are inline by default */
        border: none;     /* Reset default border */
        height: 80vh;     /* Viewport-relative units */
        width: 95vw;
    }
</style>