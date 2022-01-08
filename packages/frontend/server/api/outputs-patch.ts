import type { IncomingMessage, ServerResponse } from 'http'
import axios from 'axios'
import { streamToString } from '~/utils/streamToString'
import { getBaseUrlFromQuery } from '~/utils/getBaseUrlFromQuery'

export default async (req: IncomingMessage, res: ServerResponse) => {
  let requestBody: string = await  streamToString(req) as string
  let baseUrl = getBaseUrlFromQuery(req)

  if (!baseUrl) {
    res.statusCode = 400
    res.end('Missing baseUrl')
    return
  }

  let axiosInstance = axios.create({ baseURL: `http://${baseUrl}`})

  return (await axiosInstance.patch('/outputs', JSON.parse(requestBody))).data
}
