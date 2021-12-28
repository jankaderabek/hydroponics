import { Controller, Get } from '@nestjs/common'
import { createMainLightOutput } from '~/outputs/factory/main-light.output.factory'
import { createSeedLightLightOutput } from '~/outputs/factory/seeds-light.output.factory'
import { createWaterPumpOutput } from '~/outputs/factory/water-pump.output.factory'
import { Output } from '~/outputs/output'

@Controller('outputs')
export class OutputsController {
  readonly #waterPumpOutput: Output
  readonly #mainLightOutput: Output
  readonly #seedsLightOutput: Output

  readonly #waterPumpActiveDurationInMinutes: number = 15

  constructor() {
    this.#waterPumpOutput = createWaterPumpOutput()
    this.#mainLightOutput = createMainLightOutput()
    this.#seedsLightOutput = createSeedLightLightOutput()
  }

  @Get()
  public getOutputsStatus() {
    const currentDate = new Date()

    return {
      outputs: {
        waterPump: this.#waterPumpOutput.isActiveForDate(currentDate),
        mainLight: this.#mainLightOutput.isActiveForDate(currentDate),
        seedsLight: this.#seedsLightOutput.isActiveForDate(currentDate),
      },
    }
  }
}
