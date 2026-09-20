import Vue from 'vue'
import VueRouter from 'vue-router'
import Home from '@/components/Home'
import Profile from '@/components/Profile'
import Settings from '@/components/Settings'
import Devices from '@/components/Devices'
import DeviceAdd from '@/components/DeviceAdd'
import Login from '@/components/Login'
import Edit from '@/components/Edit'
import UserPair from '@/components/UserPair'

Vue.use(VueRouter)

const routes = [
  {
    path: '/',
    name: 'Home',
    component: Home
  },
  {
    path: '/edit/:friend_info',
    name: 'Edit',
    component: Edit
  },
  {
    path: '/profile',
    name: 'Profile',
    component: Profile
  },
  {
    path: '/set',
    name: 'Settings',
    component: Settings
  },
  {
    path: '/devices',
    name: 'Devices',
    component: Devices
  },
  {
    path: '/deviceadd',
    name: 'DeviceAdd',
    component: DeviceAdd
  },
  {
    path: '/login',
    name: 'Login',
    component: Login
  },
  {
    path: '/userpair',
    name: 'UserPair',
    component: UserPair
  },
  {
    path: '*',
    redirect: '/Home'
  }
]

const router = new VueRouter({
  routes
})

export default router

