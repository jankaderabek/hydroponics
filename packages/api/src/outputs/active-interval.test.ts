import { describe, expect, it } from 'vitest'
import { ActiveInterval } from '~/outputs/active-interval'

describe('Output`s active interval', () => {
  const activeInterval: ActiveInterval = new ActiveInterval(19, 0, 19, 15)

  it('is active for given date', () => {
    const date: Date = new Date()
    date.setHours(19)
    date.setMinutes(13)

    expect(activeInterval.isActiveForDate(date)).toBeTruthy()
  })

  it('is not active for given date', () => {
    const date: Date = new Date()
    date.setHours(19)
    date.setMinutes(16)

    expect(activeInterval.isActiveForDate(date)).toBeFalsy()
  })
})
