import React from "react";
import useFiles from "../../hooks/useFiles";

const FilesContext = React.createContext(null);

export const FilesProvider = ({ children }) => {
    const value = useFiles();
    return (
        <FilesContext.Provider value={value}>
            {children}
        </FilesContext.Provider>
    );
};

export const useFilesContext = () => {
    return React.useContext(FilesContext);
};
