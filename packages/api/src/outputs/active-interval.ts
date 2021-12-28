import { isWithinInterval, set } from 'date-fns'

export class ActiveInterval {
  readonly #fromHours
  readonly #fromMinutes
  readonly #toHours
  readonly #toMinutes

  constructor(fromHours, fromMinutes, toHours, toMinutes) {
    this.#fromHours = fromHours
    this.#fromMinutes = fromMinutes
    this.#toHours = toHours
    this.#toMinutes = toMinutes
  }

  isActiveForDate(date: Date): boolean {
    return isWithinInterval(date, {
      start: set(date, { hours: this.fromHours, minutes: this.fromMinutes }),
      end: set(date, { hours: this.toHours, minutes: this.toMinutes }),
    })
  }

  get fromHours() {
    return this.#fromHours
  }

  get fromMinutes() {
    return this.#fromMinutes
  }

  get toHours() {
    return this.#toHours
  }

  get toMinutes() {
    return this.#toMinutes
  }
}
