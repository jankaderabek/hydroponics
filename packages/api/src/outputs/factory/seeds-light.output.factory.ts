import { Output } from '~/outputs/output'
import { ActiveInterval } from '~/outputs/active-interval'

export const createSeedLightLightOutput = (): Output => {
  return new Output([new ActiveInterval(7, 0, 19, 0)])
}
