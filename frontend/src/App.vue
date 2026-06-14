<template>
  <div class="min-h-screen bg-[#020617] text-slate-200 font-sans relative overflow-hidden flex items-center justify-center p-4 lg:p-10">
    <!-- Cinematic Lighting -->
    <div class="fixed top-[-20%] left-[-10%] w-[60vw] h-[60vw] rounded-full bg-emerald-900/20 blur-[150px] pointer-events-none"></div>
    <div class="fixed bottom-[-20%] right-[-10%] w-[60vw] h-[60vw] rounded-full bg-cyan-900/20 blur-[150px] pointer-events-none"></div>
    <div class="fixed top-[40%] left-[50%] w-[40vw] h-[40vw] -translate-x-1/2 -translate-y-1/2 rounded-full bg-blue-900/10 blur-[120px] pointer-events-none mix-blend-screen"></div>

    <main class="w-full max-w-7xl z-10 grid grid-cols-1 lg:grid-cols-12 gap-8 relative">
      <!-- Left: Master Status (Moisture) -->
      <div class="lg:col-span-5 flex flex-col gap-8 h-full">
        
        <!-- Header -->
        <div class="flex items-start justify-between">
           <div>
             <h1 class="text-4xl font-light tracking-tight text-white mb-1">Smart<span class="font-bold text-emerald-400">Farm</span></h1>
             <p class="text-slate-400 font-medium tracking-wide">Environmental Controls</p>
           </div>
           <div class="flex items-center gap-3 bg-white/5 border border-white/10 px-4 py-2 rounded-full backdrop-blur-md shadow-lg">
             <div class="w-2 h-2 rounded-full bg-emerald-400 animate-pulse shadow-[0_0_10px_#34d399]"></div>
             <span class="text-xs font-bold tracking-widest text-slate-300">LIVE</span>
           </div>
        </div>

        <!-- Moisture Liquid Orb -->
        <div class="flex-1 bg-white/[0.02] border border-white/5 rounded-[40px] p-10 flex flex-col items-center justify-center relative overflow-hidden group backdrop-blur-xl shadow-2xl min-h-[450px]">
           <div class="absolute inset-0 bg-gradient-to-b from-transparent to-black/40 pointer-events-none"></div>
           
           <h2 class="text-sm font-semibold text-slate-400 z-10 mb-8 uppercase tracking-[0.2em]">Soil Hydration</h2>
           
           <!-- The Orb -->
           <div class="relative w-72 h-72 rounded-full border border-white/10 shadow-[0_0_50px_rgba(0,0,0,0.5)] p-2 backdrop-blur-md z-10 flex items-center justify-center transform group-hover:scale-105 transition-transform duration-700 ease-out">
              <div class="absolute inset-0 rounded-full bg-slate-900/80 shadow-[inset_0_10px_20px_rgba(0,0,0,0.5)]"></div>
              
              <!-- Liquid container -->
              <div class="absolute inset-3 rounded-full overflow-hidden mask-circle bg-slate-950">
                <!-- Water Waves -->
                <div class="absolute w-[250%] h-[250%] opacity-80 left-[-75%] transition-all duration-1000 ease-in-out liquid-wave"
                     :class="moistureColorClass.wave1"
                     :style="{ top: `${100 - moisture}%` }">
                </div>
                <div class="absolute w-[250%] h-[250%] opacity-50 left-[-75%] transition-all duration-1000 ease-in-out liquid-wave-2"
                     :class="moistureColorClass.wave2"
                     :style="{ top: `${100 - moisture + 3}%` }">
                </div>
                <!-- Top Light Reflection -->
                <div class="absolute top-0 left-0 w-full h-1/2 bg-gradient-to-b from-white/10 to-transparent rounded-t-full pointer-events-none"></div>
              </div>

              <!-- Text Overlay -->
              <div class="relative z-20 text-center drop-shadow-[0_2px_10px_rgba(0,0,0,1)] flex flex-col items-center justify-center mt-4">
                 <div class="flex items-start">
                   <span class="text-8xl font-black text-white tracking-tighter" :style="{ textShadow: '0 0 40px rgba(255,255,255,0.3)' }">{{ moisture }}</span>
                   <span class="text-3xl text-white/80 font-bold mt-2">%</span>
                 </div>
                 <div class="h-6 mt-2">
                   <p class="text-xs font-bold uppercase tracking-[0.3em]" :class="moistureColorClass.text">{{ moistureStatusText }}</p>
                 </div>
              </div>
           </div>
        </div>
      </div>

      <!-- Right: Pump & Chart -->
      <div class="lg:col-span-7 flex flex-col gap-8 h-full">
        <!-- Pump Control Card (Skeuomorphic/Modern toggle) -->
        <div class="bg-white/[0.02] border border-white/5 rounded-[40px] p-8 lg:p-10 flex items-center justify-between backdrop-blur-xl shadow-2xl relative overflow-hidden transition-all duration-500"
             :class="isPumpOn ? 'border-cyan-500/30 shadow-[0_0_50px_rgba(6,182,212,0.15)] bg-cyan-900/10' : ''">
           
           <div class="absolute inset-0 bg-gradient-to-r from-cyan-500/10 to-blue-500/10 transition-opacity duration-700" :class="isPumpOn ? 'opacity-100' : 'opacity-0'"></div>
           
           <div class="relative z-10 flex gap-6 items-center">
             <div class="w-20 h-20 rounded-[24px] flex items-center justify-center transition-all duration-700 shadow-xl"
                  :class="isPumpOn ? 'bg-gradient-to-br from-cyan-400 to-blue-500 text-white shadow-[0_0_30px_rgba(6,182,212,0.5)] scale-110' : 'bg-slate-800 border border-white/10 text-slate-500'">
                <svg class="w-10 h-10 transition-all duration-500" :class="{'animate-[spin_3s_linear_infinite]': isPumpOn}" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                  <path v-if="!isPumpOn" stroke-linecap="round" stroke-linejoin="round" stroke-width="1.5" d="M13 10V3L4 14h7v7l9-11h-7z"></path>
                  <path v-else stroke-linecap="round" stroke-linejoin="round" stroke-width="1.5" d="M12 3v1m0 16v1m9-9h-1M4 12H3m15.364 6.364l-.707-.707M6.343 6.343l-.707-.707m12.728 0l-.707.707M6.343 17.657l-.707.707M16 12a4 4 0 11-8 0 4 4 0 018 0z"></path>
                </svg>
             </div>
             <div>
               <h2 class="text-2xl font-bold text-white mb-1 tracking-wide">Irrigation Pump</h2>
               <p class="text-slate-400 text-sm font-medium transition-colors duration-500" :class="{'text-cyan-300': isPumpOn}">{{ isPumpOn ? 'Water is currently flowing' : 'System is standing by' }}</p>
             </div>
           </div>

           <!-- Premium Toggle -->
           <div class="relative z-10">
             <button @click="togglePump" class="relative w-36 h-16 rounded-full p-2 cursor-pointer transition-all duration-500 outline-none flex items-center shadow-inner group overflow-hidden"
                     :class="isPumpOn ? 'bg-gradient-to-r from-cyan-500 to-blue-500 shadow-[0_0_25px_rgba(6,182,212,0.4)]' : 'bg-slate-900 border border-slate-700'">
                <div class="absolute inset-0 w-full h-full bg-white/20 -translate-x-full group-hover:animate-[shimmer_2s_infinite]"></div>
                
                <div class="absolute font-black text-sm uppercase tracking-widest text-white transition-all duration-500"
                     :class="isPumpOn ? 'left-6 opacity-100' : 'left-10 opacity-0'">ON</div>
                     
                <div class="w-12 h-12 rounded-full bg-white shadow-[0_5px_15px_rgba(0,0,0,0.3)] transform transition-transform duration-500 ease-spring flex items-center justify-center z-10 relative"
                     :class="isPumpOn ? 'translate-x-[80px]' : 'translate-x-0'">
                     <div class="w-4 h-4 rounded-full transition-colors duration-500" :class="isPumpOn ? 'bg-cyan-500 shadow-[0_0_10px_rgba(6,182,212,0.8)]' : 'bg-slate-300'"></div>
                </div>

                <div class="absolute right-5 font-black text-sm uppercase tracking-widest text-slate-500 transition-all duration-500"
                     :class="!isPumpOn ? 'opacity-100' : 'opacity-0'">OFF</div>
             </button>
           </div>
        </div>

        <!-- Chart -->
        <div class="flex-1 bg-white/[0.02] border border-white/5 rounded-[40px] p-8 lg:p-10 backdrop-blur-xl shadow-2xl flex flex-col relative overflow-hidden min-h-[350px]">
           <div class="flex justify-between items-end mb-8 relative z-10">
             <div>
               <h2 class="text-xl font-bold text-white tracking-wide">Hydration Trends</h2>
               <p class="text-sm text-slate-400 mt-1 font-medium">Real-time moisture fluctuation</p>
             </div>
             <div class="text-right flex items-center gap-4">
               <div class="text-right">
                 <p class="text-[10px] text-slate-400 font-bold uppercase tracking-[0.2em] mb-1">Peak</p>
                 <span class="text-xl font-light text-white">{{ peakMoisture }}%</span>
               </div>
               <div class="h-8 w-px bg-white/10"></div>
               <div class="text-right">
                 <p class="text-[10px] text-emerald-400 font-bold uppercase tracking-[0.2em] mb-1">Current</p>
                 <span class="text-3xl font-light text-white">{{ moisture }}%</span>
               </div>
             </div>
           </div>

           <!-- Chart Gradient Background -->
           <div class="absolute bottom-0 left-0 w-full h-1/2 bg-gradient-to-t from-emerald-500/5 to-transparent pointer-events-none"></div>

           <div class="flex-1 w-full relative z-10">
             <Line v-if="chartData.labels.length > 0" :data="chartData" :options="chartOptions" />
             <div v-else class="absolute inset-0 flex items-center justify-center">
               <span class="text-slate-500 text-sm tracking-[0.2em] font-semibold uppercase animate-pulse flex items-center gap-3">
                 <svg class="w-5 h-5 animate-spin text-emerald-500" fill="none" viewBox="0 0 24 24"><circle class="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" stroke-width="4"></circle><path class="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"></path></svg>
                 Gathering Telemetry...
               </span>
             </div>
           </div>
        </div>
      </div>
    </main>
  </div>
</template>

<script setup>
import { ref, computed, onMounted, onUnmounted, shallowRef } from 'vue'
import { Line } from 'vue-chartjs'
import { Chart as ChartJS, CategoryScale, LinearScale, PointElement, LineElement, Title, Tooltip, Filler } from 'chart.js'

ChartJS.register(CategoryScale, LinearScale, PointElement, LineElement, Title, Tooltip, Filler)

const moisture = ref(0)
const isPumpOn = ref(false)
const rawHistory = ref([])
let pollingInterval = null
let historyPollingInterval = null

// Compute peak moisture from history
const peakMoisture = computed(() => {
  if (rawHistory.value.length === 0) return 0;
  return Math.max(...rawHistory.value.map(item => item.moisture))
})

// Dynamic text based on moisture
const moistureStatusText = computed(() => {
  if (moisture.value < 30) return 'Critically Dry'
  if (moisture.value < 60) return 'Drying'
  if (moisture.value <= 80) return 'Optimal'
  return 'Saturated'
})

// Dynamic colors for the liquid waves
const moistureColorClass = computed(() => {
  if (moisture.value < 30) return {
    wave1: 'bg-gradient-to-t from-rose-600 to-rose-400',
    wave2: 'bg-gradient-to-t from-rose-700 to-orange-500',
    text: 'text-rose-400'
  }
  if (moisture.value < 60) return {
    wave1: 'bg-gradient-to-t from-amber-500 to-yellow-300',
    wave2: 'bg-gradient-to-t from-amber-600 to-orange-400',
    text: 'text-amber-400'
  }
  return {
    wave1: 'bg-gradient-to-t from-emerald-500 to-cyan-400',
    wave2: 'bg-gradient-to-t from-emerald-600 to-blue-500',
    text: 'text-emerald-400'
  }
})

const chartData = ref({
  labels: [],
  datasets: []
})

// Minimalist, premium chart styling
const chartOptions = {
  responsive: true,
  maintainAspectRatio: false,
  animation: { duration: 1000, easing: 'easeOutQuart' },
  scales: {
    y: {
      beginAtZero: true,
      max: 100,
      grid: { color: 'rgba(255, 255, 255, 0.02)', drawBorder: false },
      ticks: { color: '#475569', font: { family: 'Inter, sans-serif', size: 11, weight: '500' }, padding: 10 }
    },
    x: {
      grid: { display: false },
      ticks: { color: '#475569', font: { family: 'Inter, sans-serif', size: 11, weight: '500' }, maxTicksLimit: 6, padding: 10 }
    }
  },
  plugins: {
    tooltip: {
      backgroundColor: 'rgba(2, 6, 23, 0.9)',
      titleColor: '#94a3b8',
      titleFont: { size: 11, family: 'Inter', weight: 'normal' },
      bodyColor: '#fff',
      bodyFont: { size: 14, family: 'Inter', weight: 'bold' },
      borderColor: 'rgba(255,255,255,0.05)',
      borderWidth: 1,
      padding: 12,
      displayColors: false,
      callbacks: {
        title: (context) => `Time: ${context[0].label}`,
        label: (context) => `Moisture: ${context.parsed.y}%`
      }
    }
  },
  elements: {
    point: { radius: 0, hitRadius: 20, hoverRadius: 6, backgroundColor: '#fff', borderWidth: 2, borderColor: '#10b981' },
    line: { tension: 0.4, borderCapStyle: 'round' } // Smooth curves
  },
  interaction: {
    mode: 'index',
    intersect: false,
  },
}

const API_URL = 'http://localhost:8080/api'

const fetchData = async () => {
  try {
    const resMoisture = await fetch(`${API_URL}/sensor/latest`)
    if (resMoisture.ok) {
      const data = await resMoisture.json()
      moisture.value = data.moisture
    }
    
    const resPump = await fetch(`${API_URL}/pump/status`)
    if (resPump.ok) {
      const data = await resPump.json()
      isPumpOn.value = data.status === 'ON'
    }
  } catch (err) {
    console.error('Error fetching data:', err)
  }
}

const fetchHistory = async () => {
  try {
    const res = await fetch(`${API_URL}/sensor/history`)
    if (res.ok) {
      const history = await res.json()
      rawHistory.value = history
      const sortedHistory = [...history].reverse()
      
      const labels = sortedHistory.map(item => {
        const d = new Date(item.created_at)
        return `${d.getHours().toString().padStart(2, '0')}:${d.getMinutes().toString().padStart(2, '0')}`
      })
      const dataPoints = sortedHistory.map(item => item.moisture)

      // Create gradient for chart in canvas (mocked via standard colors here, but you can add canvas gradient logic)
      chartData.value = {
        labels,
        datasets: [{
          label: 'Soil Moisture',
          data: dataPoints,
          borderColor: '#10b981', // emerald-500
          backgroundColor: 'rgba(16, 185, 129, 0.1)',
          borderWidth: 3,
          fill: true,
          pointHoverBackgroundColor: '#10b981',
          pointHoverBorderColor: '#fff',
          pointHoverBorderWidth: 3,
          pointHoverRadius: 6,
        }]
      }
    }
  } catch(err) {
    console.error('Error fetching history:', err)
  }
}

const togglePump = async () => {
  const endpoint = isPumpOn.value ? '/pump/off' : '/pump/on'
  try {
    await fetch(`${API_URL}${endpoint}`, { method: 'POST' })
    isPumpOn.value = !isPumpOn.value // Optimistic UI update
    fetchData() // Re-sync
  } catch (err) {
    console.error('Error toggling pump:', err)
  }
}

onMounted(() => {
  fetchData()
  fetchHistory()
  pollingInterval = setInterval(fetchData, 2000)
  historyPollingInterval = setInterval(fetchHistory, 5000)
})

onUnmounted(() => {
  if (pollingInterval) clearInterval(pollingInterval)
  if (historyPollingInterval) clearInterval(historyPollingInterval)
})
</script>

<style>
@import url('https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700;800;900&display=swap');

body {
  font-family: 'Inter', sans-serif;
  background-color: #020617; /* slate-950 */
}

/* Custom CSS Liquid Wave Animation */
.mask-circle {
  -webkit-mask-image: -webkit-radial-gradient(white, black);
}

.liquid-wave {
  border-radius: 38%;
  animation: spin 7s linear infinite;
}

.liquid-wave-2 {
  border-radius: 42%;
  animation: spin 5s linear infinite;
}

@keyframes spin {
  0% { transform: rotate(0deg); }
  100% { transform: rotate(360deg); }
}

@keyframes shimmer {
  100% { transform: translateX(100%); }
}

/* Custom Spring Transition */
.ease-spring {
  transition-timing-function: cubic-bezier(0.34, 1.56, 0.64, 1);
}
</style>

