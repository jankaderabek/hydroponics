import { AxiosInstance } from 'axios'
import { useNuxtApp } from '#app'

export const useAxios = (): AxiosInstance => {
  const { $axios } = useNuxtApp()
  return $axios as AxiosInstance
}
