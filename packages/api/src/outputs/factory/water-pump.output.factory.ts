import { Output } from '~/outputs/output'
import { ActiveInterval } from '~/outputs/active-interval'

const waterPumpActiveDurationInMinutes = 15

export const createWaterPumpOutput = (): Output => {
  const waterPumpActiveIntervals = []

  for (let i = 6; i <= 22; i++) {
    waterPumpActiveIntervals.push(
      new ActiveInterval(i, 0, i, waterPumpActiveDurationInMinutes),
    )
  }

  return new Output(waterPumpActiveIntervals)
}
