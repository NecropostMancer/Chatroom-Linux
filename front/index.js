import {createApp} from 'vue';

//import {createRouter,createWebHashHistory} from 'vue-router';

import App from './App.vue';

//import './src/style/common.scss';

//import ChatMain from './src/components/ChatMain.vue'
//import Login from './src/components/Login.vue'

import {BootstrapVue,BootstrapVueIcons} from 'bootstrap-vue'
import 'bootstrap/dist/css/bootstrap.css'
import 'bootstrap-vue/dist/bootstrap-vue.css'

const app = createApp(App)

app.use(BootstrapVue)
app.use(BootstrapVueIcons)

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
