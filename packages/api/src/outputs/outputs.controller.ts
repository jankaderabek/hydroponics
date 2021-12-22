import { Controller, Get } from '@nestjs/common'

@Controller('outputs')
export class OutputsController {

  @Get()
  public getOutputsStatus () {
    return {
      outputs: {
        light_relay: false
      }
    }
  }
}
