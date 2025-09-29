const translations = {
    about: {
        en: {
            title: "About",
            documentation: "Documentation",
            about: "A wireless network planning tool. It is designed to help network engineers and planners evaluate and optimize the performance of wireless networks through accurate simulations and detailed analysis."
        },
        es: {
            title: "Acerca de",
            documentation: "Documentación",
            about: "Herramienta de planificación de redes inalámbricas. Diseñada para ayudar a los ingenieros y planificadores de redes a evaluar y optimizar el rendimiento de las redes inalámbricas mediante simulaciones precisas y análisis detallados."
        }
    },
    error:{
        en: {
            app_restarted: "Application restarted",
            error_reported: "Error reported",
            critical_error: "A critical error occurred",
            error_msg_1: "The application has detected a critical error and cannot continue running. Our team is working to resolve it as soon as possible.",
            error_msg_2: "Please try again by restarting the application or submit a report to help us identify the issue.",
            send_report: "Send report",
            restart_app: "Restart application",
            los_error: "An error occurred while computing LOS",
            request_error: "An error occurred while processing the request"
        },
        es: {
            app_restarted: "Aplicación reiniciada",
            error_reported: "Error reportado",
            critical_error: "Ocurrió un error crítico",
            error_msg_1: "La aplicación ha detectado un error crítico y no puede continuar ejecutándose. Nuestro equipo está trabajando para resolverlo lo antes posible.",
            error_msg_2: "Vuelva a intentarlo nuevamente reiniciando la aplicación o envíe un reporte para ayudarnos a encontrar el problema.",
            send_report: "Enviar reporte",
            restart_app: "Reiniciar aplicación",
            los_error: "Ocurrió un error al calcular LOS",
            request_error: "Ocurrió un error al procesar la solicitud",
            geojeson_parsing_error: "Error al parsear GeoJSON"
        }
    },
    charts: {
        en: {
            distance: "Distance (m)",
            elevation: "Elevation (m)",
            reference: "Reference"
        },
        es: {
            distance: "Distancia (m)",
            elevation: "Elevación (m)",
            reference: "Referencia"
        }
    },
    model: {
        en: {
            unsupported_file: "Unsupported file format",
            file_read_error: "Error reading file",
            geojson_parsing_error: "Error parsing GeoJSON",
            cannot_delete_file: "Cannot delete file from server",
            file_deletion_success: "File deleted successfully"
        },
        es: {
            unsupported_file: "Formato de archivo no soportado",
            file_read_error: "Error al leer el archivo",
            cannot_delete_file: "No se pudo eliminar el archivo del servidor",
            file_deletion_success: "Archivo eliminado exitosamente"
        }
    },
    help: {
        en: {
            title: "Help"
        },
        es: {
            title: "Ayuda"
        }
    },
    los_results_modal: {
        en: {
            title: "Line of Sight (LOS) Calculation",
            point_1: "Point 1",
            point_2: "Point 2",
            lat: "lat",
            lon: "lon",
            elev: "elev",
            distance: "Distance",
            line_of_sight: "Line of sight",
            line_of_sight_fresnel_60pct: "Line of sight (Fresnel 60%)",
            yes: "Yes",
            no: "No",
            terrain_profile: "Terrain profile"
        },
        es: {
            title: "Cálculo de línea de vista (LOS)",
            point_1: "Punto 1",
            point_2: "Punto 2",
            lat: "lat",
            lon: "lon",
            elev: "elev.",
            distance: "Distancia",
            line_of_sight: "Línea de vista",
            line_of_sight_fresnel_60pct: "Línea de vista (Fresnel 60%)",
            yes: "Si",
            no: "No",
            terrain_profile: "Perfil de terreno"
        }
    },
    network_result_modal: {
        en: {
            title: "Connectivity Test",
            total_devices: "Total devices",
            gateways: "Gateways",
            connected_devices: "Connected devices",
            disconnected_devices: "Disconnected devices",
            connectivity_percentage: "Connectivity percentage",
            total_link_distance: "Total link distance"
        },
        es: {
            title: "Test de conectividad",
            total_devices: "Dispositivos totales",
            gateways: "Gateways",
            connected_devices: "Dispositivos conectados",
            disconnected_devices: "Dispositivos no conectados",
            connectivity_percentage: "Porcentaje de conectividad",
            total_link_distance: "Distancia total de enlaces"
        }
    },
    controls: {
        en: {
            dropzone_text: "Drag and drop files here or select from your directory",
            files_status: "Files status",
            elevation_map_uploaded: "Elevation map uploaded",
            elevation_map_local: "Elevation map in local mode",
            features_uploaded: "Features file uploaded",
            features_local: "Features file in local mode",
            connectivity_test: "Connectivity test",
            remove_elevation: "Remove elevation map",
            remove_features: "Remove features file",
            compute_los: "Compute LOS",
            reset_points: "Reset points",
            remove_features: "Remove features",
            remove_elevation: "Remove elevation",
            evaluate_network: "Evaluate network",
            run_solver: "Run solver"
        },
        es: {
            dropzone_text: "Arrastrar y soltar archivos aquí o seleccione desde su directorio",
            files_status: "Estado de los archivos:",
            elevation_map_uploaded: "Mapa de elevación cargado",
            elevation_map_local: "Mapa de elevación en modo local",
            features_uploaded: "Geometrías cargadas",
            features_local: "Geometrías en modo local",
            connectivity_test: "Test de conectividad",
            remove_elevation: "Eliminar mapa de elevación",
            remove_features: "Eliminar archivo de características",
            compute_los: "Evaluar LOS",
            reset_points: "Reiniciar puntos",
            remove_features: "Quitar geometrías",
            remove_elevation: "Quitar altimetría",
            evaluate_network: "Evaluar red",
            run_solver: "Ejecutar solver"
        }
    },
    nodes_table: {
        en: {
            type: "Type",
            end_device: "End device",
            gateway: "Gateway",
            id: "Id",
            position: "Pos.",
            connections: "Connections",
            empty_table_1: "No nodes to display.",
            empty_table_2: "Upload a points file or add nodes on the map."
        },
        es: {
            type: "Tipo",
            end_device: "Disp. final",
            gateway: "Gateway",
            id: "Id",
            position: "Pos.",
            connections: "Conexiones",
            empty_table_1: "No hay nodos para mostrar.",
            empty_table_2: "Cargue un archivo de puntos o agregue nodos en el mapa."
        }
    },
    navigation: {
        en: {
            home: "Map",
            details: "Details",
            about: "About",
            help: "Help",
            switch_language: "Language changed to English",
            alt: "UK Flag"
        },
        es: {
            home: "Mapa",
            details: "Detalles",
            about: "Acerca de",
            help: "Ayuda",
            switch_language: "Idioma cambiado a Español",
            alt: "Bandera de Argentina"
        }
    }
};

const dictionaries = ["es", "en"];

const dictionary = dictionaries.reduce((acc, lang) => {
    acc[lang] = {};
    return acc;
}, {});

Object.keys(translations).forEach(key => {
    dictionaries.forEach(lang => {
        dictionary[lang][key] = translations[key][lang];
    });
});

export default dictionary;