<template>
  <div class="max-w-7xl mx-auto my-3 sm:px-6 lg:px-8">
    <div class="bg-white shadow overflow-hidden sm:rounded-lg">
      <div class="px-4 py-5 sm:px-6 flex items-center">
        <div class="w-8 h-8 rounded-full" :class="{ 'bg-red-600': !active, 'bg-green-600': active } "></div>

        <h3 class="text-lg leading-6 font-medium text-gray-900 px-4">
          {{ title }}
        </h3>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
defineProps({
  active: Boolean,
  title: String
})
</script>
