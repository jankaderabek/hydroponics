import { Module } from '@nestjs/common'
import { AppController } from './app.controller'
import { AppService } from './app.service'
import { OutputsModule } from './outputs/outputs.module'

@Module({
  imports: [OutputsModule],
  controllers: [AppController],
  providers: [AppService],
})
export class AppModule {}
