import { IncomingMessage } from 'http'

export function getBaseUrlFromQuery(req: IncomingMessage): string | null {
  let url: URL = new URL('http://test.fs/outputs' + req.url ?? '')

  return url.searchParams.get('baseUrl')
}
