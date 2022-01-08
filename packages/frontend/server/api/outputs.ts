import type { IncomingMessage, ServerResponse } from 'http'
import axios from 'axios'
import { getBaseUrlFromQuery } from '~/utils/getBaseUrlFromQuery'

export default async (req: IncomingMessage, res: ServerResponse) => {
  let baseUrl = getBaseUrlFromQuery(req)

  if (!baseUrl) {
    res.statusCode = 400
    res.end('Missing baseUrl')
    return
  }

  let axiosInstance = axios.create({ baseURL: `http://${baseUrl}`})

  return (await axiosInstance.get('/outputs')).data
}
