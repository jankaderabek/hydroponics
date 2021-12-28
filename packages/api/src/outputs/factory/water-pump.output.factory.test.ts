import { describe, expect, it } from 'vitest'
import { createWaterPumpOutput } from '~/outputs/factory/water-pump.output.factory'

describe('Waterpump output', () => {
  const waterPumpOutput = createWaterPumpOutput()

  it('has correct count of active intervals', () => {
    const date: Date = new Date()
    date.setHours(19)
    date.setMinutes(13)

    expect(waterPumpOutput.activeIntervals).toHaveLength(17)
  })
})
