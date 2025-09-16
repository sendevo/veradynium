import { createContext, useContext } from "react";
import useFiles from "../../hooks/useFiles";

const FilesContext = createContext();

export const FilesProvider = ({ children }) => {
    const fileOps = useFiles();
    return (
        <FilesContext.Provider value={fileOps}>
            {children}
        </FilesContext.Provider>
    );
};

export const useFilesContext = () => useContext(FilesContext);