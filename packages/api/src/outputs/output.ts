import { ActiveInterval } from '~/outputs/active-interval'

export class Output {
  readonly #activeIntervals: ActiveInterval[]

  constructor(activeIntervals: ActiveInterval[]) {
    this.#activeIntervals = activeIntervals
  }

  isActiveForDate(date: Date): boolean {
    return this.#activeIntervals.some((i) => i.isActiveForDate(date))
  }

  get activeIntervals(): ActiveInterval[] {
    return this.#activeIntervals
  }
}
