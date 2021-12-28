import { describe, expect, it } from 'vitest'
import { ActiveInterval } from '~/outputs/active-interval'
import { Output } from '~/outputs/output'

describe('Output', () => {
  const output = new Output([
    new ActiveInterval(8, 0, 9, 15),
    new ActiveInterval(19, 0, 19, 15),
  ])

  it('is output active for given date', () => {
    const date: Date = new Date()

    date.setHours(8)
    date.setMinutes(43)

    expect(output.isActiveForDate(date)).toBeTruthy()

    date.setHours(19)
    date.setMinutes(13)

    expect(output.isActiveForDate(date)).toBeTruthy()
  })

  it('is not active output for given date', () => {
    const date: Date = new Date()
    date.setHours(19)
    date.setMinutes(16)

    expect(output.isActiveForDate(date)).toBeFalsy()
  })
})
