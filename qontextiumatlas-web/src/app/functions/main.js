import { Chart } from 'chart.js/auto';
import 'chartjs-adapter-date-fns';

/* Init Global variables */
let chartInsts = [];

const checkStruct = (rawDataArray) => {
  return (rawDataArray && rawDataArray[0]) ? true : false;
}

const filterNull = (rawDataArray) => {
  if (checkStruct(rawDataArray)) return [];
  const filteredDataArray = rawDataArray.filter(data => (data.data != null));
  return filteredDataArray;
}

//Specific structure to the rawDataArray state object
const getStateObjectData = (data) => {
  if ((data) && (typeof data !== 'object')) return false;
  const keys = Object.keys(data);
  let type;
  (data[keys[0]] !== '1' && data[keys[0]] !== '0') ? type = 0 : type = 1;
  return {'data': data[keys[0]], 'state': type === 0 ? (data[keys[1]] !== '0') : (data[keys[0]] !== '0'), 'type': type}
}

//Input: [{"data": "value", "createdAt": "timeString"}],
//Output: {"min": value, "max": value}
function getPeakSensorValue(dataArray) {
  if (!checkStruct(dataArray)) return {"min": "Unknown", "max": "Unknown"};
  let max = dataArray[0].data, min = dataArray[0].data;
  for (let i = 1; i < dataArray.length; i++) {
      if (dataArray[i].data < min) {
          min = dataArray[i].data;
      }
      if (dataArray[i].data > max) {
          max = dataArray[i].data;
      }
  }
  return {"min": min, "max": max};
}

//Input [{"x": "timeString", "y": "value"}],
//Output: [{"x": "timeString", "y": "AverageData"}]
function getAverageChartData(chartData, chartTimeRange) {
    const chartTimeStamp = chartTimeRange.end.getTime() - chartTimeRange.start.getTime();
    const maxAllowedTimeStamp = chartTimeStamp / 500;
    let groupedData = [chartData[0]];
    let timeStampSum = 0;
    let pointValueSum = 0, pointCount = 0;
    for (let i = 1; i < chartData.length-1; i++) {
        const pointTimeStamp = chartData[i].x.getTime() - chartData[i-1].x.getTime();
        pointCount++;
        if ((pointTimeStamp < maxAllowedTimeStamp) && (timeStampSum < maxAllowedTimeStamp)) {
            timeStampSum = timeStampSum + pointTimeStamp;
            pointValueSum = pointValueSum + chartData[i].y;
        } else {
            groupedData.push({"x": chartData[i].x, "y": pointValueSum / pointCount});
            timeStampSum = 0;
            pointValueSum = 0;
            pointCount = 0;
        }
    }
    groupedData.push(chartData.at(-1));
  
    return groupedData;
}

//Input [{"x": "timeString", "y": "value"}],
//Output: [{"x": "timeString", "y": "groupedValue"}]
function getGroupedChartData(chartData, chartTimeRange) {
    const chartTimeStamp = chartTimeRange.end.getTime() - chartTimeRange.start.getTime();
    const maxAllowedTimeStamp = chartTimeStamp / 1500;
    let groupedData = [chartData[0]];
    let timeStampSum = 0;
    let isChunkPositive = false;
    let timeAtChunkStart = 0;
    for (let i = 1; i < chartData.length-1; i++) {
        if (timeAtChunkStart == 0) timeAtChunkStart = chartData[i].x.getTime();
        const pointTimeStamp = chartData[i].x.getTime() - chartData[i-1].x.getTime();
        if ((pointTimeStamp < maxAllowedTimeStamp) && (timeStampSum < maxAllowedTimeStamp)) {
            timeStampSum = timeStampSum + pointTimeStamp;
            isChunkPositive = isChunkPositive || (chartData[i].y == 1);
        } else {
            groupedData.push({"x": timeAtChunkStart, "y": isChunkPositive ? 1 : 0});
            timeStampSum = 0;
            isChunkPositive = false;
            timeAtChunkStart = 0;
        }
    }
    groupedData.push(chartData.at(-1));
  
    return groupedData;
}

//Input: '/endpointName',
//Output_: [{"data": "value"}] or [{"data": {Object}}]
async function fetchRawDataArray(endpointName, setter, timeRange) {
  try {
      const response = await fetch(`${endpointName}?timeStart=${timeRange.start.getTime()}&timeEnd=${timeRange.end.getTime()}`, {
          method: 'GET',
          headers: {
            "Content-Type": "application/json",
          },
      });
      const rawDataArray = await response.json();

      if (!checkStruct(rawDataArray)) return;
      setter(rawDataArray);
  } catch (error) {
      console.error('Error fetching rawDataArray: ', error);
  }
}

//Input: [{"data": "value", "createdAt": "timeString"}] or [{"data": {Object}, "createdAt": "timeString"}]
//Output: [{"y": value, "x": {timeObj}}]
function getChartData(dataArray) {
  const chartData = dataArray.map(data => {
      const stateObjectData = getStateObjectData(data.data);
      if (stateObjectData) {
          return {"x": new Date(data.createdAt), "y": stateObjectData.state ? 1 : 0};
      } else {
          return {"x": new Date(data.createdAt), "y": parseFloat(data.data)};
      }
  });
  return chartData;
}

//Input: [{"data": "value", "createdAt": "timeString"}] or [{"data": {Object}, "createdAt": "timeString"}]
//Output: {"before": [dataArray], "after": [dataArray]}
function getMissingData(dataArray, chartTimeRange) {
    const debug = false; if (debug) {console.log('B.inMissingData'); console.log(dataArray); console.log(chartTimeRange);}
  const delay = 60 * 1000, predictedValue = "0";
  const padding = (chartTimeRange.end.getTime() - chartTimeRange.start.getTime()) / 25;
  const delyedTimeRange = {"start": chartTimeRange.start, "end": new Date(chartTimeRange.end.getTime() - delay)};
  const defaultMissingData = [
      {"data": predictedValue, "createdAt": new Date(chartTimeRange.start).toString()},
      {"data": predictedValue, "createdAt": new Date(chartTimeRange.end).toString()} 
  ];

  let missingData = {"before": [], "after": []};
  if (!checkStruct(dataArray)) {
      missingData.after = defaultMissingData;
      if (debug) console.log('no data');
      return missingData;
  }
  const dataTimeRange = {"start": new Date(dataArray[0].createdAt), "end": new Date(dataArray.at(-1).createdAt)};

  if (dataTimeRange.start <= delyedTimeRange.start && dataTimeRange.end >= delyedTimeRange.end) {
      //Normal behavior
      if (debug) console.log('normal behavior');
  }
  /*
  [ds, de]
  [cs, ce  ]
  */
  else if (dataTimeRange.start <= delyedTimeRange.start && dataTimeRange.end < delyedTimeRange.end) {
      missingData.after = [
          {"data": dataArray.at(-1).data, "createdAt": new Date(dataTimeRange.end).toString()},
          {"data": predictedValue, "createdAt": new Date(dataTimeRange.end.getTime()+padding).toString()},
          {"data": predictedValue, "createdAt": new Date(chartTimeRange.end).toString()}
      ];
      if (debug) console.log('after');
  }
  /*
    [ds, de]
  [  cs, ce]
  */
  else if (dataTimeRange.start > delyedTimeRange.start && dataTimeRange.end >= delyedTimeRange.end) {
      missingData.before = [
          {"data": predictedValue, "createdAt": new Date(chartTimeRange.start).toString()},
          {"data": predictedValue, "createdAt": new Date(dataTimeRange.start.getTime()-padding).toString()},
          {"data": dataArray[0].data, "createdAt": new Date(dataTimeRange.start).toString()},
      ];
      if (debug) console.log('before');
  }
  /*
    [ds, de]
  [  cs, ce  ]
  */
  else if (dataTimeRange.start > delyedTimeRange.start && dataTimeRange.end < delyedTimeRange.end) {
      missingData.before = [
          {"data": predictedValue, "createdAt": new Date(chartTimeRange.start).toString()},
          {"data": predictedValue, "createdAt": new Date(dataTimeRange.start.getTime()-padding).toString()},
          {"data": dataArray[0].data, "createdAt": new Date(dataTimeRange.start).toString()},
      ];
      missingData.after = [
          {"data": dataArray.at(-1).data, "createdAt": new Date(dataTimeRange.end).toString()},
          {"data": predictedValue, "createdAt": new Date(dataTimeRange.end.getTime()+padding).toString()},
          {"data": predictedValue, "createdAt": new Date(chartTimeRange.end).toString()}
      ]
      if (debug) console.log('both sides');
  } else {
      missingData.after = defaultMissingData;
      if (debug) console.log('other');
  }

  if (debug) {console.log(missingData); console.log('E.inMissingData');}
  return missingData;
}

function setChart(chartId, chartType, initialChartData, initialChartTimeRange) {
  const ctx = document.getElementById(chartId).getContext('2d');
  let options, data;

  if (chartType == 0) {
      options = {
          layout: {
              padding: {
                  top: 15,
              }
          },
          plugins: {
              legend: { display: false },
          },
          animation: {
              duration: 0,
          },
          responsive: true,
          maintainAspectRatio: false,
          scales: {
              x: {
                  type: 'time',
                  display: false,
                  grid: {
                      display: false
                  },
                  ticks: {
                      display: false
                  },
                  min: initialChartTimeRange.start,
                  max: initialChartTimeRange.end
              },
              y: {
                  display: false,
                  grid: {
                      display: false
                  },
                  ticks: {
                      display: false
                  },
                  min: 0,
              }
          }
      }
      data = {
          datasets: [
              { 
                  data: initialChartData.missingDataBefore,
                  fill: false,
                  tension: 0,
                  pointRadius: 0,
                  borderColor: function(context) {
                      const gradient = ctx.createLinearGradient(0, 0, context.chart.width, 0);
                      gradient.addColorStop(0, 'rgba(31, 31, 31, 0)');
                      gradient.addColorStop(0.5, 'rgb(228, 48, 48)');
                      gradient.addColorStop(1, 'rgba(31, 31, 31, 0)');
                      return gradient;
                  },
              },
              { 
                  data: initialChartData.sensorData,
                  fill: false,
                  tension: 0.5,
                  pointRadius: 0,
                  borderColor: function(context) {
                      const gradient = ctx.createLinearGradient(0, 0, context.chart.width, 0);
                      gradient.addColorStop(0, 'rgba(31, 31, 31, 0)');
                      gradient.addColorStop(0.5, 'rgba(48, 228, 142, 1)');
                      gradient.addColorStop(1, 'rgba(31, 31, 31, 0)');
                      return gradient;
                  },
              },
              { 
                  data: initialChartData.missingDataAfter,
                  fill: false,
                  tension: 0,
                  pointRadius: 0,
                  borderColor: function(context) {
                      const gradient = ctx.createLinearGradient(0, 0, context.chart.width, 0);
                      gradient.addColorStop(0, 'rgba(31, 31, 31, 0)');
                      gradient.addColorStop(0.5, 'rgb(228, 48, 48)');
                      gradient.addColorStop(1, 'rgba(31, 31, 31, 0)');
                      return gradient;
                  },
              },
          ]
      };
  }
  else if (chartType == 1) {
      options = {
          layout: {
              padding: {
                  top: 15,
              }
          },
          plugins: {
              legend: { display: false },
          },
          animation: {
              duration: 0,
          },
          responsive: true,
          maintainAspectRatio: true,
          scales: {
              x: {
              type: 'time',
                  display: true,
              grid: {
                  display: false
              },
              ticks: {
                  display: true,
                  maxTicksLimit: 8,
              },
              min: initialChartTimeRange.start,
              max: initialChartTimeRange.end
              },
              y: {
                  display: true,
                  grid: {
                      display: false
                  },
                  ticks: {
                      display: true
                  },
                  min: 0,
              }
          }
      };
      data = {
          datasets: [
          { 
              data: initialChartData.missingDataBefore,
              fill: true,
              tension: 0,
              pointRadius: 0,
              borderWidth: 1,
              borderColor: "rgb(228, 48, 48)",
              backgroundColor: function(context) {
                const gradient = ctx.createLinearGradient(0, 0, 0, context.chart.height);
                gradient.addColorStop(0.5, 'rgba(228, 48, 48, 0.1)');
                gradient.addColorStop(1, 'rgba(31, 31, 31, 0)');
                return gradient;
              },
          },
          { 
              data: initialChartData.sensorData,
              fill: true,
              tension: 0.2,
              pointRadius: 1,
              borderWidth: 1,
              borderColor: "rgba(48, 228, 142, 1)",
              backgroundColor: function(context) {
                const gradient = ctx.createLinearGradient(0, 0, 0, context.chart.height);
                gradient.addColorStop(0.5, 'rgba(48, 228, 142, 0.1)');
                gradient.addColorStop(1, 'rgba(31, 31, 31, 0)');
                return gradient;
              },
          },
          { 
              data: initialChartData.missingDataAfter,
              fill: true,
              tension: 0,
              pointRadius: 0,
              borderWidth: 1,
              borderColor: "rgb(228, 48, 48)",
              backgroundColor: function(context) {
                const gradient = ctx.createLinearGradient(0, 0, 0, context.chart.height);
                gradient.addColorStop(0.5, 'rgba(228, 48, 48, 0.1)');
                gradient.addColorStop(1, 'rgba(31, 31, 31, 0)');
                return gradient;
              },
          },
          ]
      };
  }

  chartInsts[chartId] = new Chart(chartId, {
      type: "line",
      data: data,
      options: options
  });
  return chartInsts[chartId];
}

async function updateChart(chartId, chartData, chartTimeRange) {
    //Update chart
    chartInsts[chartId].data.datasets[0].data = chartData.missingDataBefore;
    chartInsts[chartId].data.datasets[1].data = chartData.sensorData;
    chartInsts[chartId].data.datasets[2].data = chartData.missingDataAfter;
    chartInsts[chartId].options.scales.x.min = chartTimeRange.start;
    chartInsts[chartId].options.scales.x.max = chartTimeRange.end;
    chartInsts[chartId].update();
}

/* -------------------------------------------------------------------------------------------------------------------- */

const getDisplayData = (data, dataTimeRange) => {
    if (!checkStruct(data)) return {'value': 'Unknown', 'peaks': {'min': 'Unknown', 'max': 'Unknown'}};
    const missingDataValue = getMissingData(data, dataTimeRange).after;
    const dataValue =  checkStruct(missingDataValue) ? 'Unknown' : data.at(-1).data;
    const dataPeaks = getPeakSensorValue(data);

    return {'value': dataValue, 'peaks': dataPeaks};
}

const getDisplayStateData = (state, isRealTime, dataTimeRange) => {
    if (!checkStruct(state) || state[0].data == 'Unknown') return {'value': 'Unknown', 'state': '-1'};
    const missingDataValue = !isRealTime ? false : getMissingData(state, dataTimeRange).after;
    const stateObjectData = getStateObjectData(state.at(-1).data);
    let stateDataValue, stateDataState;
    if (stateObjectData) {
        stateDataValue = stateObjectData.type == 0 ? stateObjectData.data : stateObjectData.state;
        stateDataState = checkStruct(missingDataValue) ? '-1' : stateObjectData.state;
    } else {
        stateDataValue = (state.at(-1).data == 0 ? false : true);
        stateDataState = checkStruct(missingDataValue) ? '-1' : (state.at(-1).data == 0 ? false : true);
    }

    return {'value': stateDataValue, 'state': stateDataState};
}

export {
    checkStruct,
    filterNull,
    getDisplayData,
    getDisplayStateData,
    setChart,
    getChartData,
    getMissingData,
    updateChart,
    fetchRawDataArray,
    getAverageChartData,
    getGroupedChartData,
    getStateObjectData,
}