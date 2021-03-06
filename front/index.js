import {createApp} from 'vue';

//import {createRouter,createWebHashHistory} from 'vue-router';

import App from './App.vue';

import './src/style/common.scss';

//import ChatMain from './src/components/ChatMain.vue'
//import Login from './src/components/Login.vue'

import "bootstrap/dist/css/bootstrap.min.css";
import "bootstrap";

const app = createApp(App)


//const routes = [
//    {path: '/main', component: ChatMain},
//    {path: '/', component: Login},
//]

//const router = createRouter({
//    history : createWebHashHistory(),
//    routes,
//})

//app.use(router);
import net from 'net'

app.config.globalProperties.$socket = net.Socket();
app.config.globalProperties.$socket.connect({
        host: '127.0.0.1',
        port: 12345
    },() => {
        console.log("connected to server");
        
    })
app.config.globalProperties.$userState = {
    loginUserName:"",
    tokenFromServer:"",
    loginShowName:""
}
app.mount('#app');
