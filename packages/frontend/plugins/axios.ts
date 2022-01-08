import axios, { AxiosInstance } from 'axios'
import { defineNuxtPlugin } from '#app'

export default defineNuxtPlugin((nuxtApp) => {
  const axiosInstance = axios.create()

  nuxtApp.provide('axios', axiosInstance)
})

declare module '#app' {
  interface NuxtApp {
    $axios: AxiosInstance
  }
}
