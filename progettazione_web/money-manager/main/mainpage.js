const mobile = screen.width <= 768;
console.log(mobile);

const longWeekDays = [
  "Lunedì",
  "Martedì",
  "Mercoledì",
  "Giovedì",
  "Venerdì",
  "Sabato",
  "Domenica",
];
const shortWeekDays = ["Lun", "Mar", "Mer", "Gio", "Ven", "Sab", "Dom"];

Highcharts.chart("grafico-barre", {
  chart: {
    type: "column",
  },
  title: {
    text: "Media Spese Settimanali",
    style: {
      color: "#2caffe",
      fontWeight: "bold",
    },
  },
  xAxis: {
    categories: mobile ? shortWeekDays : longWeekDays,
    title: {
      text: mobile ? null : "Giorni della Settimana",
    },
  },
  yAxis: {
    min: 0,
    title: {
      text: mobile ? null : "Euro",
      align: "high",
    },
    labels: {
      format: "€{value}",
    },
  },
  tooltip: {
    valueSuffix: " €",
  },
  plotOptions: {
    bar: {
      dataLabels: {
        enabled: true,
        format: "€{y}",
      },
    },
  },
  series: [
    {
      name: "Solid spesi",
      data: [12, 18, 9, 14, 22.1, 35, 28.5],
    },
  ],
});
Highcharts.chart("grafico-entrate", {
  chart: {
    type: "line",
  },
  title: {
    text: mobile ? "Entrate" : "Entrate durante la settimana",
    style: {
      color: "#2caffe",
    },
  },
  xAxis: {
    categories: mobile ? shortWeekDays : longWeekDays,
  },
  yAxis: {
    title: {
      text: mobile ? null : "Euro",
    },
  },
  tooltip: {
    valueSuffix: " €",
  },
  series: [
    {
      name: "Entrate",
      data: [50, 75, 60, 80, 90, 120, 100],
    },
  ],
});

// Grafico Uscite
Highcharts.chart("grafico-uscite", {
  chart: {
    type: "line",
  },
  title: {
    text: mobile ? "Uscite" : "Uscite durante la settimana",
    style: {
      color: "#2caffe",
    },
  },
  xAxis: {
    categories: mobile ? shortWeekDays : longWeekDays,
  },
  yAxis: {
    title: {
      text: mobile ? null : "Euro",
    },
  },
  tooltip: {
    valueSuffix: " €",
  },
  series: [
    {
      name: "Uscite",
      data: [20, 30, 25, 40, 35, 50, 45],
    },
  ],
});
