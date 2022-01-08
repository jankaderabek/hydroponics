<template>
  <div>
    <main>
      <OutputState
        title="Main lights"
        :active="outputsState.mainLights"
        @toggle="toggleOutput('mainLights')"
      />

      <OutputState
        title="Seeds lights"
        :active="outputsState.seedsLights"
        @toggle="toggleOutput('seedsLights')"
      />

      <OutputState
        title="Water pump"
        :active="outputsState.waterPump"
        @toggle="toggleOutput('waterPump')"
      />

      <div class="max-w-7xl mx-auto my-3 px-6 lg:px-8">
        <button
          @click="loadState"
          type="submit"
          class="inline-flex justify-center py-2 px-4 border border-transparent shadow-sm text-sm font-medium rounded-md text-white bg-indigo-600 hover:bg-indigo-700 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-indigo-500"
        >
          Reload data
        </button>
      </div>
    </main>
  </div>
</template>

<script setup lang="ts">
import OutputState from '~/components/OutputState.vue'
import { useAxios } from '~/composables/axios'
import { ref } from '@vue/reactivity'
import { onMounted } from '@vue/runtime-core'
import { useCookie } from '#app'

interface OutputsState {
  waterPump: boolean
  mainLights: boolean
  seedsLights: boolean
}

const axios = useAxios()

const outputsState = ref<OutputsState>({
  waterPump: false,
  mainLights: false,
  seedsLights: false
})

const serverAddress = useCookie('serverAddress')

onMounted(() => {
  loadState()
  setInterval(() => loadState(), 10000)
})

const loadState = async () => {
  const axiosResponse = await axios.get<OutputsState>('/api/outputs', { params: { baseUrl: serverAddress.value } })

  outputsState.value = axiosResponse.data
}

const toggleOutput = async (output: keyof OutputsState) => {
  const axiosResponse = await axios.patch<OutputsState>(
      '/api/outputs-patch',
      {
        [output]: !outputsState.value[output]
      },
      { params: { baseUrl: serverAddress.value } }
  )

  outputsState.value = axiosResponse.data
}

</script>
